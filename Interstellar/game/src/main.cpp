#include <iostream>
#include <algorithm>

#include "Interstellar/Interstellar.hpp"
#include "Interstellar/Graphics/Core/ITexture.hpp"
#include "Interstellar/Core/Logging.hpp"
#include "Graphics/OpenGL/OpenGLGraphics.hpp"
#include "Interstellar/Input/IInputManager.hpp"
#include "Interstellar/Input/IKeyboardManager.hpp"
#include "Interstellar/Input/IMouseManager.hpp"
#include "Interstellar/Graphics/Core/IShader.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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

#include "Interstellar/Config/JsonImpl/ElementDatabase.hpp"
//#include "Interstellar/Data/ElementData.hpp"
int main() {
    glm::vec2 triangleOffset = glm::vec2(0.0f);
    glm::vec2 dragStart = glm::vec2(0.0f);
    bool dragging = false;
    float zoom = 1.0f; // Default zoom level

    const auto s_Logger = Interstellar::Core::Logger(LOG_CATEGORY);

    s_Logger.LogInfo("Interstellar Engine Initialisation...");
    Interstellar::Config::JsonImpl::ElementDatabase elementDb;
    if (!elementDb.loadFromFile("assets/data/elements.json")) {
        s_Logger.LogError("Failed to load element database from 'assets/data/elements.json'.");
    }
    else {
        auto all = elementDb.all();
        for (const auto& [symbol, e] : all) {
            s_Logger.LogInfo("Element {}: {} ({}), mass={} abundance {} ", 
                symbol, 
                e.name, e.symbol, e.atomicMass,
                e.abundance
            );
        }
    }


    //Interstellar::Core::Logger s_Logger(LOG_CATEGORY);
    // ==== TEMPORARY TEST CODE ====
    constexpr int windowWidth = 1200;
    constexpr int windowHeight = 860;

    auto graphics = std::make_unique<Interstellar::Graphics::OpenGL::OpenGLGraphics>();
    if (!graphics->Initialise(windowWidth, windowHeight, false)) {
        std::cerr << "[Error] Failed to initialise OpenGL graphics backend.\n";
        return -1;
    }

    auto window = static_cast<GLFWwindow*>(graphics->GetNativeWindow());
    auto input = Interstellar::Input::IInputManager::Create(window);
    auto& keyboard = input->GetKeyboardManager();
    auto& mouse = input->GetMouseManager();

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
        input->Update(); // Refresh input states

        float scrollY = static_cast<float>(mouse.GetScrollOffsetY());
        if (scrollY != 0.0f) {

            float zoomDelta = scrollY * 0.5f;
            zoom = zoomDelta;
            zoom = std::clamp(zoom, 0.1f, 5000.0f);
        }

        // Keyboard movement
        float moveSpeed = 0.5f * static_cast<float>(deltaTime);
        if (keyboard.IsKeyDown(GLFW_KEY_LEFT))  triangleOffset.x -= moveSpeed;
        if (keyboard.IsKeyDown(GLFW_KEY_RIGHT)) triangleOffset.x += moveSpeed;
        if (keyboard.IsKeyDown(GLFW_KEY_UP))    triangleOffset.y += moveSpeed;
        if (keyboard.IsKeyDown(GLFW_KEY_DOWN))  triangleOffset.y -= moveSpeed;

        // Mouse drag
        if (mouse.IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            glm::vec2 current = glm::vec2(mouse.GetX(), mouse.GetY());

            if (!dragging) {
                dragging = true;
                dragStart = current;
            }

            if (mouse.IsDragging()) {
                glm::vec2 delta = (current - dragStart) / glm::vec2(windowWidth, windowHeight);
                delta.y *= -1.0f; // Invert Y for OpenGL
                triangleOffset += delta * 2.0f;
                dragStart = current;
            }
        }
        else {
            dragging = false;
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
                s_Logger.LogWarn("Texture '{}' has no valid native handle!");// , texture->GetName());
            }
        }
        else {
            s_Logger.LogWarn("Texture was not created (nullptr).");
        }

        // Send u_Offset to the shader
        //auto glShader = std::static_pointer_cast<Interstellar::Graphics::OpenGL::OpenGLShader>(shader);
        GLuint programID = static_cast<GLuint>(reinterpret_cast<uintptr_t>(shader->GetNativeHandle()));
        glUseProgram(programID);
        GLint offsetLoc = glGetUniformLocation(programID, "u_Offset");
        if (offsetLoc >= 0) {
            glUniform2f(offsetLoc, triangleOffset.x, triangleOffset.y);
        }
        GLint zoomLoc = glGetUniformLocation(programID, "u_Zoom");
        if (zoomLoc >= 0) {
            glUniform1f(zoomLoc, zoom);
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

    s_Logger.LogInfo(std::string("Game closed with success."));

    return 0;
}