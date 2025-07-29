#include <iostream>

#include "Interstellar/Interstellar.hpp"
#include "Interstellar/Graphics/Core/ITexture.hpp"
#include "Interstellar/Core/Logging.hpp"
#include "Graphics/OpenGL/OpenGLGraphics.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace {
    constexpr auto LOG_CATEGORY = Interstellar::Core::LOG_INIT;
    constexpr const char* TEXTURE_PATH = "assets/textures/texture_01.png";

    // Vertex layout: position (x, y, z), UV (u, v)
    constexpr float vertices[] = {
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
        0.0f,  0.5f, 0.0f,   0.5f, 1.0f,
    };

    unsigned int indices[] = { 0, 1, 2 };
}

int main() {

    const auto s_Logger = Interstellar::Core::Logger(LOG_CATEGORY);

    //Interstellar::Core::Logger s_Logger(LOG_CATEGORY);
    // ==== TEMPORARY TEST CODE ====
    constexpr int windowWidth = 1200;
    constexpr int windowHeight = 860;

    auto graphics = std::make_unique<Interstellar::Graphics::OpenGL::OpenGLGraphics>();
    if (!graphics->Initialise(windowWidth, windowHeight, false)) {
        std::cerr << "[Error] Failed to initialise OpenGL graphics backend.\n";
        return -1;
    }

    auto shader = graphics->CreateShader("TriangleShader");
    auto mesh = graphics->CreateMesh(vertices, sizeof(vertices), indices, sizeof(indices));
    auto pipeline = graphics->CreatePipeline(shader);
    auto texture = graphics->CreateTexture(TEXTURE_PATH);

    if (!shader || !mesh || !pipeline || !texture) {
        if (!shader)   s_Logger.LogError("Shader failed to compile or load.");
        if (!mesh)     s_Logger.LogError("Mesh failed to initialize.");
        if (!pipeline) s_Logger.LogError("Pipeline creation failed.");
        if (!texture)  s_Logger.LogError("Texture loading failed.");
        return -2;
    }

    // Temporary FPS tracking
    double lastTime = glfwGetTime();
    double fpsAvg = 0.0;
    static double timeAccumulator = 0.0;

    while (!graphics->ShouldClose()) {
        // Frame timing
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        double fps = 1.0 / deltaTime;
        fpsAvg = ( fps + ( fpsAvg * 9.0 ) ) / 10.0;
        timeAccumulator += deltaTime;

        if (timeAccumulator >= 1.0) {
          s_Logger.LogDebug("FPS: {} | Avg: {} ",fps, fpsAvg);
          timeAccumulator = 0.0;
        }

        // Render
        graphics->BeginFrame();

        // Manually bind texture to unit 0
        glActiveTexture(GL_TEXTURE0);
        if (texture) {
            auto native = texture->GetNativeHandle();
            if (native) {
                glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<uintptr_t>(native)));
            }
            else {
                s_Logger.LogWarn("Texture '{}' has no valid native handle!", texture->GetName());
            }
        }
        else {
            s_Logger.LogWarn("Texture was not created (nullptr).");
        }

        graphics->SubmitMesh(mesh, pipeline);
        graphics->EndFrame();
    }

    graphics->Shutdown();
    glfwTerminate();
    // ==== END OF TEMPORARY TEST CODE ====

    // ==== GAME INIT (NOT YET REACHED) ====
    const auto genericLogger = Interstellar::Core::Logger();
    
    Interstellar::Game game;

    try {
        game.loadConfig();
        game.buildComponents();
        game.start();
        game.shutdown();
    }
    catch (const std::exception& e) {
        genericLogger.LogCritical(std::string("Game error: ") + e.what());
        return 99;
    }

    return 0;
}