#include <glad/glad.h>
#include <iostream>

#include "OpenGLGraphics.hpp"

using namespace Interstellar::Graphics;
using namespace Interstellar::Graphics::OpenGL;

OpenGLGraphics::OpenGLGraphics() = default;

OpenGLGraphics::~OpenGLGraphics() {
    Shutdown();
}

bool OpenGLGraphics::Initialize(void* windowHandle, uint32_t width, uint32_t height, bool vsync) {
    if (!glfwInit()) {
        std::cerr << "[OpenGL] Failed to initialize GLFW.\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = static_cast<GLFWwindow*>(windowHandle);
    if (!m_Window) {
        std::cerr << "[OpenGL] No valid window provided.\n";
        return false;
    }

    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[OpenGL] Failed to initialize GLAD.\n";
        return false;
    }

    m_Vsync = vsync;
    glfwSwapInterval(vsync ? 1 : 0);

    std::cout << "[OpenGL] Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "[OpenGL] Version: " << glGetString(GL_VERSION) << "\n";

    glViewport(0, 0, width, height);

    return true;
}

void OpenGLGraphics::Shutdown() {
    // Nothing to do here; context is destroyed by GLFW
}

void OpenGLGraphics::BeginFrame() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLGraphics::EndFrame() {
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

void OpenGLGraphics::Resize(uint32_t width, uint32_t height) {
    glViewport(0, 0, width, height);
}

std::string OpenGLGraphics::GetRendererName() const {
    return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
}

Core::GraphicsAPI OpenGLGraphics::GetAPI() const {
    return Core::GraphicsAPI::OpenGL;
}

// We'll implement these soon
std::shared_ptr<Core::IMesh> OpenGLGraphics::CreateMesh(const void*, size_t, const void*, size_t) {
    return nullptr;
}
std::shared_ptr<Core::ITexture> OpenGLGraphics::CreateTexture(const std::string&) {
    return nullptr;
}
std::shared_ptr<Core::IShader> OpenGLGraphics::CreateShader(const std::string&) {
    return nullptr;
}
std::shared_ptr<Core::IRenderPipeline> OpenGLGraphics::CreatePipeline(std::shared_ptr<Core::IShader>) {
    return nullptr;
}
void OpenGLGraphics::SubmitMesh(std::shared_ptr<Core::IMesh>, std::shared_ptr<Core::IRenderPipeline>) {
    // NOP for now
}
