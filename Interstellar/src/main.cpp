#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Interstellar/Engine/Engine.hpp"
#include "Interstellar/Logging/Logging.hpp"
#include <Interstellar/Input/Input.hpp>

#include "Interstellar/Renderers/OpenGL/OpenGLGraphics.hpp"

#include "Interstellar/Engine/cameras/CameraRig3D.hpp"
#include "Interstellar/Graphics/renderers/StarsRenderer.hpp"

// Deterministic universe service
#include "Interstellar/Universe/query/UniverseService.hpp"
#include "Interstellar/Universe/Universe3DRecipe.hpp"
#include "Interstellar/Universe/AABB.hpp"
#include "Interstellar/Universe/Seed.hpp"

using Interstellar::Engine::Engine;
using Interstellar::Logging::LogInit;
using namespace Interstellar::Input;
using namespace Interstellar::Universe;

int main() {
    // ==== WINDOW ====
    int windowWidth = 960, windowHeight = 640;

    auto graphics = std::make_unique<Interstellar::Renderers::OpenGL::OpenGLGraphics>();
    if (!graphics->Initialise(windowWidth, windowHeight, /*vsync*/false)) {
        std::cerr << "[Error] Failed to initialise OpenGL graphics backend.\n";
        return -1;
    }
    GLFWwindow* window = static_cast<GLFWwindow*>(graphics->GetNativeWindow());

    // ==== INPUT ====
    InputConfig cfg; cfg.nativeWindow = window; cfg.backend = InputConfig::Backend::GLFW;
    auto input = InputSystem::Create(cfg);
    auto& keyboard = input->keyboard();
    auto& mouse = input->mouse();

    // ==== CAMERA RIG ====
    Interstellar::Engine::CameraRig3D cam; // defaults okay

    // ==== UNIVERSE SERVICE (deterministic) ====
    const Seed64 masterSeed = seedFromString("Interstellar-Alpha-001");
    Universe3DRecipe recipe;
    recipe.sectorSize = 256.0f;
    recipe.starDensity = 4.0e-5f; // tweak density here
    recipe.jitter = 0.35f;

    UniverseService universe(masterSeed, recipe);

    // ==== STARS RENDERER ====
    Interstellar::Graphics::StarsRenderer stars(*graphics);
    // Optional tweak:
    Interstellar::Graphics::StarVisualParams vis{};
    vis.minBrightness = 0.01f; vis.maxBrightness = 1.4f;
    vis.minPx = 1.0f; vis.maxPx = 140.0f;
    vis.brightNear = 10000.0f; vis.brightFar = 200000.0f;
    vis.baseSizePxAtUnitDepth = 1.0f;
    stars.setVisuals(vis);

    // HUD title
    auto setTitle = [&](std::size_t starCount) {
        std::ostringstream ss;
        ss.setf(std::ios::fixed); ss.precision(2);
        ss << "Interstellar Universe  |  X " << cam.center.x
            << "  Y " << cam.center.y
            << "  Z " << cam.z
            << "  FOV " << cam.fovDeg
            << "  Stars " << starCount;
        glfwSetWindowTitle(window, ss.str().c_str());
        };

    // ==== GAME LOOP ====
    Engine engine;
    engine.run(
        // --- SIM ---
        [&](double dt) {
            input->pump();
            input->beginFrame(dt);

            cam.handleInput(keyboard, mouse, windowWidth, windowHeight, dt);
            cam.update(dt);

            input->endFrame();

            int w, h; glfwGetFramebufferSize(window, &w, &h);
            if (w != windowWidth || h != windowHeight) {
                windowWidth = std::max(w, 1); windowHeight = std::max(h, 1);
            }
        },

        // --- RENDER ---
        [&](double /*ft*/) {
            graphics->BeginFrame();

            const float aspect = float(windowWidth) / float(windowHeight);
            const glm::mat4 VP = cam.VP(aspect);
            const glm::vec3 eye = cam.eye();

            // Build a slab around current camera depth (reduces popping)
            const float halfH = std::tan(glm::radians(cam.fovDeg * 0.5f)) * cam.z;
            const float halfW = halfH * aspect;
            const float slabHalf = std::max(50.0f, cam.z * 0.08f);

            AABB3 slab3{
              glm::vec3(cam.center.x - halfW, cam.center.y - halfH, cam.z - slabHalf),
              glm::vec3(cam.center.x + halfW, cam.center.y + halfH, cam.z + slabHalf)
            };

            UniverseQueryFilters filters; // stars = true
            UniverseQueryResult  result;
            result.clear();
            universe.queryAABB(slab3, filters, result);

            // Draw stars
            stars.draw(result.stars, VP, eye, windowHeight, cam.fovDeg);

            graphics->EndFrame();
            setTitle(result.stars.size());
        },

        // --- EXIT ---
        [&] { return graphics->ShouldClose(); }
    );

    graphics->Shutdown();
    glfwTerminate();
    return 0;
}
