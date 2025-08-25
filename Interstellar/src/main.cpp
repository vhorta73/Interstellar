#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

#include "Interstellar/Engine/Camera2DExtras.hpp"
#include "Interstellar/Universe/FieldGenerator.hpp"
#include "Interstellar/Core/fmt_optional.hpp"
#include "Interstellar/ECS/World.hpp"
#include "Interstellar/Engine/Engine.hpp"
#include "Interstellar/Engine/FrameTimer.hpp"
#include "Interstellar/Renderers/OpenGL/GLInstancedSubmit.hpp"

#include "Interstellar/Engine/ScopedZone.hpp"
#include "Interstellar/Engine/Camera2D.hpp"
#include "Interstellar/Graphics/IMaterial.hpp"
#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IShader.hpp"
#include "Interstellar/Graphics/ITexture.hpp"
#include "Interstellar/Interstellar.hpp"
#include "Interstellar/Logging/Logging.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLGraphics.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLShader.hpp"
#include "Interstellar/Simulation/Systems/MovementSystem.hpp"
#include <Interstellar/Input/Input.hpp>
#include "Interstellar/Universe/SectorStreamer.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace {
    constexpr const char* TEXTURE_PATH = "assets/textures/texture_01.png";

    // Vertex layout: position (x, y, z), UV (u, v)
    constexpr float vertices[] = {
        -0.05f, -0.05f, 0.0f,  0.0f, 0.0f,
         0.05f, -0.05f, 0.0f,  1.0f, 0.0f,
         0.0f,  0.05f, 0.0f,  0.5f, 1.0f,
    };

    unsigned int indices[] = { 0, 1, 2 };

    // Critically-damped exponential smoothing using half-life (seconds).
    inline float exp_smooth(float current, float target, float halflife, double dt) {
        if (halflife <= 0.0f) return target;
        const float k = std::exp2f(static_cast<float>(-dt) / halflife); // 2^(-dt/hl)
        return target + (current - target) * k;
    }
    inline glm::vec2 exp_smooth(glm::vec2 current, glm::vec2 target, float halflife, double dt) {
        return { exp_smooth(current.x, target.x, halflife, dt),
                 exp_smooth(current.y, target.y, halflife, dt) };
    }

    enum class ViewMode : uint8_t { SpaceMap = 0, PlanetGod, FirstPerson, UndergroundSlice };

    struct ModeParams {
        float posHalflife;     // how quickly the camera centers converges
        float zoomHalflife;    // how quickly zoom converges
        float panSpeed;        // keyboard pan speed in world units/sec
        float minZoom, maxZoom;
        float defaultZoom;
    };

    inline ModeParams paramsFor(ViewMode m) {
        switch (m) {
        case ViewMode::SpaceMap:       return { 0.12f, 0.18f,  0.7f,  0.05f, 5000.0f,  1.0f };
        case ViewMode::PlanetGod:      return { 0.10f, 0.15f,  0.4f,  0.02f,  200.0f,  3.0f };
        case ViewMode::FirstPerson:    return { 0.07f, 0.10f,  1.2f,  0.20f,   10.0f,  8.0f };
        case ViewMode::UndergroundSlice:return{ 0.10f, 0.16f,  0.3f,  0.02f,  100.0f,  4.0f };
        default:                       return { 0.12f, 0.18f,  0.7f,  0.05f, 5000.0f,  1.0f };
        }
    }
}

int main() {
    using Interstellar::Engine::Engine;
    using Interstellar::ECS::World;
    using Interstellar::Simulation::MovementSystem;
    using Interstellar::Logging::LogInit;
    using namespace Interstellar::Renderers::OpenGL;
    using namespace Interstellar::Input;

    // ==== WINDOW ====
    constexpr int windowWidth = 4680;
    constexpr int windowHeight = 2400;

    auto graphics = std::make_unique<Interstellar::Renderers::OpenGL::OpenGLGraphics>();
    if (!graphics->Initialise(windowWidth, windowHeight, false)) {
        std::cerr << "[Error] Failed to initialise OpenGL graphics backend.\n";
        return -1;
    }

    auto window = static_cast<GLFWwindow*>(graphics->GetNativeWindow());
    InputConfig cfg;
    cfg.nativeWindow = window;
    cfg.backend = InputConfig::Backend::GLFW;

    auto input = InputSystem::Create(cfg);
    auto build = GetBuildInfo();
    std::cout << "[Input] Active backend=" << BackendName(input->backend())
        << " | GLFW compiled: " << (build.haveGLFW ? "yes" : "no") << "\n";
    auto& keyboard = input->keyboard();
    auto& mouse = input->mouse();

    // ==== GPU RESOURCES ====
    auto shader = graphics->CreateShader("TriangleShader");
    auto mesh = graphics->CreateMesh(vertices, sizeof(vertices), indices, sizeof(indices));
    auto pipeline = graphics->CreatePipeline(shader);
    auto texture = graphics->CreateTexture(TEXTURE_PATH);
    auto material = pipeline->CreateMaterial();

    if (!shader || !mesh || !pipeline || !texture) {
        if (!shader)   LogInit().LogError("Shader failed to compile or load.");
        if (!mesh)     LogInit().LogError("Mesh failed to initialize.");
        if (!pipeline) LogInit().LogError("Pipeline creation failed.");
        if (!texture)  LogInit().LogError("Texture loading failed.");
        return -2;
    }

    // ==== CAMERA RIG (smooth) ====
    Interstellar::Engine::Camera2D camera;    // current camera actually used for rendering
    glm::vec2 targetCenter = camera.center;   // smoothed towards this
    float     targetZoom = 1.0f;            // smoothed towards this

    ViewMode  mode = ViewMode::SpaceMap;
    ModeParams mParams = paramsFor(mode);

    // Helpers for edge-triggered mode switches
    auto keyPressedEdge = [&](KeyCode code, bool& latch)->bool {
        bool down = keyboard.isDown(code);
        bool fired = down && !latch;
        latch = down;
        return fired;
        };
    bool latch1 = false, latch2 = false, latch3 = false, latch4 = false;

    // Keep your seeded star field
    Interstellar::Universe::FieldGenerator field(/*seed*/0xDEADBEEFCAFEBABEULL, /*grid*/0.12f, /*jitter*/0.45f);

    // temp reuse buffer for instancing offsets
    std::vector<float> instanceOffsets;

    // Simple mouse-drag filter (reuse your MouseFilter if you prefer)
    MouseFilter dragFilter{ 0.5f };

    // ==== ECS demo world (still available) ====
    Engine engine;
    World  world;
    world.create(0.0f, 0.0f, 0.1f, 0.0f);

    engine.run(
        // --- SIMULATION ---
        [&](double dt) {
            input->pump();
            input->beginFrame(dt);

            // ----- View mode switches (1..4) -----
            if (keyPressedEdge(KeyCode::Num1, latch1)) { mode = ViewMode::SpaceMap;        mParams = paramsFor(mode); targetZoom = mParams.defaultZoom; }
            if (keyPressedEdge(KeyCode::Num2, latch2)) { mode = ViewMode::PlanetGod;       mParams = paramsFor(mode); targetZoom = mParams.defaultZoom; }
            if (keyPressedEdge(KeyCode::Num3, latch3)) { mode = ViewMode::FirstPerson;     mParams = paramsFor(mode); targetZoom = mParams.defaultZoom; }
            if (keyPressedEdge(KeyCode::Num4, latch4)) { mode = ViewMode::UndergroundSlice; mParams = paramsFor(mode); targetZoom = mParams.defaultZoom; }

            // ----- Keyboard pan (acts on *targets*, camera eases towards them) -----
            const float pan = mParams.panSpeed * static_cast<float>(dt);
            if (keyboard.isDown(KeyCode::ArrowLeft))  targetCenter.x += pan;
            if (keyboard.isDown(KeyCode::ArrowRight)) targetCenter.x -= pan;
            if (keyboard.isDown(KeyCode::ArrowUp))    targetCenter.y -= pan;
            if (keyboard.isDown(KeyCode::ArrowDown))  targetCenter.y += pan;

            // ----- Mouse wheel zoom (acts on target) -----
            targetZoom = WheelZoom(mouse, targetZoom, 0.15f, mParams.minZoom, mParams.maxZoom);

            // ----- Drag to pan (acts on target; dragging world -> move target opposite delta) -----
            if (mouse.isDown(MouseButton::Left)) {
                glm::vec2 d = dragFilter.apply(mouse, windowWidth, windowHeight);
                targetCenter -= d;
            }
            else {
                dragFilter.reset();
            }

            // ----- Smooth towards targets -----
            camera.center = exp_smooth(camera.center, targetCenter, mParams.posHalflife, dt);
            camera.zoom = exp_smooth(camera.zoom, targetZoom, mParams.zoomHalflife, dt);

            // Keep your demo system running
            MovementSystem(world, dt);
            input->endFrame();
        },

        // --- RENDER ---
        [&](double /*frameTime*/) {
            graphics->BeginFrame();

            // Push camera + texture
            camera.apply(*material);
            material->Set("u_Texture", texture);

            // Compute visible world AABB for streaming
            const auto vis = Interstellar::Engine::VisibleAABB(camera, windowWidth, windowHeight);

            // Generate instance positions within the visible rect
            field.generate({ vis.min, vis.max }, instanceOffsets);

            // Draw all visible points as instanced triangles
            GLInstancedSubmit::draw(*graphics, mesh, pipeline, material,
                instanceOffsets.data(),
                static_cast<int>(instanceOffsets.size() / 2));

            graphics->EndFrame();
        },

        // --- EXIT CONDITION ---
        [&] { return graphics->ShouldClose(); }
    );

    graphics->Shutdown();
    glfwTerminate();

    // ==== GAME INIT (NOT YET REACHED) ====
    Interstellar::Game game;
    try {
        game.loadConfig();
        game.buildComponents();
        game.start();
        game.shutdown();
    }
    catch (const std::exception& e) {
        LogInit().LogCritical(std::string("Game error: ") + e.what());
        return 99;
    }

    LogInit().LogInfo(std::string("Game closed with success."));
    return 0;
}
