#include "OpenGLGraphics.hpp"
#include "OpenGLMesh.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLPipeline.hpp"
#include "OpenGLTexture.hpp"
#include "Interstellar/Core/Logging.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace Interstellar::Graphics;
using namespace Interstellar::Graphics::OpenGL;

static constexpr const char* ShaderBasePath = "assets/shaders/";

static const Interstellar::Core::Logger s_Logger(Interstellar::Core::LOG_GRAPHIC);

OpenGLGraphics::OpenGLGraphics() = default;

OpenGLGraphics::~OpenGLGraphics() {
    Shutdown();
}

bool OpenGLGraphics::Initialize(uint32_t width, uint32_t height, bool vsync) {
    if (!glfwInit()) {
        s_Logger.LogError("Failed to initialize GLFW.");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(width, height, "Game Window", nullptr, nullptr);
    if (!m_Window) {
        s_Logger.LogError("Failed to create GLFW window.");
        return false;
    }

    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        s_Logger.LogError("Failed to initialize GLAD.");
        return false;
    }

    m_Vsync = vsync;
    glfwSwapInterval(vsync ? 1 : 0);

    glEnable(GL_DEPTH_TEST); // Enables Z-buffering.

    s_Logger.LogInfo("Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    s_Logger.LogInfo("Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

    glViewport(0, 0, width, height);

    return true;
}

void OpenGLGraphics::Shutdown() {
    if (m_Window) {
      glfwDestroyWindow(m_Window);
      m_Window = nullptr;
    }
    glfwTerminate();
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

std::shared_ptr<Core::IMesh> OpenGLGraphics::CreateMesh(const void* vertexData, size_t vertexSize,
    const void* indexData, size_t indexSize) {
    return std::make_shared<OpenGLMesh>(vertexData, vertexSize, indexData, indexSize);
}
std::shared_ptr<Core::ITexture> OpenGLGraphics::CreateTexture(const std::string& path) {
    return std::make_shared<OpenGLTexture>(path);
}

// TODO: Not hardcode these shaders.
std::shared_ptr<Core::IShader> OpenGLGraphics::CreateShader(const std::string& name) {
    return std::make_shared<OpenGLShader>(name,
        std::string(ShaderBasePath) + "triangle.vert",
        std::string(ShaderBasePath )+ "triangle.frag"
    );
}

std::shared_ptr<Core::IRenderPipeline> OpenGLGraphics::CreatePipeline(std::shared_ptr<Core::IShader> shader) {
    return std::make_shared<OpenGLPipeline>(shader);
}

void OpenGLGraphics::SubmitMesh(std::shared_ptr<Core::IMesh> mesh, std::shared_ptr<Core::IRenderPipeline> pipeline) {
    auto glMesh = std::static_pointer_cast<OpenGLMesh>(mesh);
    auto oglPipeline = std::static_pointer_cast<OpenGLPipeline>(pipeline);
    auto glShader = std::static_pointer_cast<OpenGLShader>(pipeline->GetShader());

    unsigned int programID = reinterpret_cast<uintptr_t>(glShader->GetNativeHandle());
    glUseProgram(programID);

    // Tell shader to use texture unit 0
    GLint textureUniformLoc = glGetUniformLocation(programID, "u_Texture");
    if (textureUniformLoc >= 0) {
        glUniform1i(textureUniformLoc, 0);
    }
    else {
        s_Logger.LogWarn("Shader '{}' is missing uniform 'u_Texture'.", glShader->GetName());
    }
    glMesh->Bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(glMesh->GetIndexCount()), GL_UNSIGNED_INT, 0);
}

bool OpenGLGraphics::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}