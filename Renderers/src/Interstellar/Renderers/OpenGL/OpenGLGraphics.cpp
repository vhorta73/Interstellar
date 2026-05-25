#include "Interstellar/Renderers/OpenGL/OpenGLGraphics.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLMesh.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLShader.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLPipeline.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLTexture.hpp"
#include "Interstellar/Graphics/IMaterial.hpp"
#include "Interstellar/Logging/Logging.hpp"
#include "Interstellar/Graphics/IMesh.hpp"
#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IShader.hpp"


#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Interstellar/Logging/Logging.hpp"

static constexpr const char* ShaderBasePath = "assets/shaders/";

namespace Interstellar::Renderers::OpenGL {

    using namespace Interstellar::Logging;
    using namespace Interstellar::Graphics;

    /**
     * @brief Constructs the OpenGLGraphics system.
     */
    OpenGLGraphics::OpenGLGraphics() = default;

    /**
     * @brief Destructor. Ensures clean shutdown of OpenGL and GLFW resources.
     */
    OpenGLGraphics::~OpenGLGraphics() {
        Shutdown();
    }

    /**
     * @brief Initializes the OpenGL context and GLFW window.
     *
     * Sets the context version, loads OpenGL functions via GLAD,
     * enables depth testing, and logs renderer information.
     *
     * @param width Initial window width.
     * @param height Initial window height.
     * @param vsync Whether to enable vertical sync.
     * @return true on successful initialization.
     */
    bool OpenGLGraphics::Initialise(uint32_t width, uint32_t height, bool vsync) {
        if (!glfwInit()) {
            logGraphic.LogError("Failed to initialize GLFW.");
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(width, height, "Game Window", nullptr, nullptr);
        if (!m_Window) {
            logGraphic.LogError("Failed to create GLFW window.");
            return false;
        }

        glfwMakeContextCurrent(m_Window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            logGraphic.LogError("Failed to initialize GLAD.");
            return false;
        }

        m_Vsync = vsync;
        glfwSwapInterval(vsync ? 1 : 0); // Enable or disable vsync

        glEnable(GL_DEPTH_TEST); // Enable depth testing
        glDisable(GL_CULL_FACE);           // optional for points
        glEnable(GL_PROGRAM_POINT_SIZE); // needed for gl_PointSize in stars.vert

        logGraphic.LogInfo("Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        logGraphic.LogInfo("Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glViewport(0, 0, width, height);
        return true;
    }

    /**
     * @brief Cleans up the OpenGL context and destroys the GLFW window.
     */
    void OpenGLGraphics::Shutdown() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
        glfwTerminate();
    }

    /**
     * @brief Begins a new frame by clearing the color and depth buffers.
     */
    void OpenGLGraphics::BeginFrame() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    /**
     * @brief Ends the current frame by swapping buffers and polling events.
     */
    void OpenGLGraphics::EndFrame() {
        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    /**
     * @brief Resizes the OpenGL viewport.
     *
     * @param width New width in pixels.
     * @param height New height in pixels.
     */
    void OpenGLGraphics::Resize(uint32_t width, uint32_t height) {
        glViewport(0, 0, width, height);
    }

    /**
     * @brief Returns the name of the current OpenGL renderer.
     * @return Renderer name as a string.
     */
    std::string OpenGLGraphics::GetRendererName() const {
        return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    }

    /**
     * @brief Returns the currently used graphics API.
     * @return Core::GraphicsAPI::OpenGL
     */
    Interstellar::Graphics::GraphicsAPI OpenGLGraphics::GetAPI() const {
        return Interstellar::Graphics::GraphicsAPI::OpenGL;
    }

    /**
     * @brief Creates a new OpenGLMesh from raw vertex and index data.
     *
     * @param vertexData Pointer to vertex buffer.
     * @param vertexSize Size of vertex buffer in bytes.
     * @param indexData Pointer to index buffer.
     * @param indexSize Size of index buffer in bytes.
     * @return Shared pointer to created IMesh.
     */
    std::shared_ptr<Interstellar::Graphics::IMesh> OpenGLGraphics::CreateMesh(
        const void* vertexData, size_t vertexSize,
        const void* indexData, size_t indexSize)
    {
        return std::make_shared<OpenGLMesh>(vertexData, vertexSize, indexData, indexSize);
    }

    /**
     * @brief Loads a texture from the given path into OpenGL.
     * @param path Filesystem path to the image.
     * @return Shared pointer to the created ITexture.
     */
    std::shared_ptr<Interstellar::Graphics::ITexture> OpenGLGraphics::CreateTexture(const std::string& path) {
        return std::make_shared<OpenGLTexture>(path);
    }

    /**
     * @brief Loads a shader from disk with hardcoded paths.
     *
     * @param name Debug name for the shader.
     * @return Shared pointer to the created IShader.
     * @todo Refactor to support dynamic shader loading paths.
     */
    std::shared_ptr<Interstellar::Graphics::IShader> OpenGLGraphics::CreateShader(const std::string& name) {
        if (name == "Stars3D") {
            return std::make_shared<OpenGLShader>(
                name,
                "assets/shaders/stars_points3d.vert",
                "assets/shaders/stars_points3d.frag"
            );
        }
        // Choose shader pair by logical name
        if (name == "Stars") {
            return std::make_shared<OpenGLShader>(
                name,
                std::string(ShaderBasePath) + "stars.vert",
                std::string(ShaderBasePath) + "stars.frag"
            );
        }

        if (name == "GalaxyBg") {
            return std::make_shared<OpenGLShader>(
                name,
                std::string(ShaderBasePath) + "galaxy_bg.vert",
                std::string(ShaderBasePath) + "galaxy_bg.frag"
            );
        }

        // Fallback: triangle demo
        return std::make_shared<OpenGLShader>(
            name,
            std::string(ShaderBasePath) + "triangle.vert",
            std::string(ShaderBasePath) + "triangle.frag"
        );
    }

    /**
     * @brief Creates a render pipeline with the specified shader.
     * @param shader The shader to use for the pipeline.
     * @return Shared pointer to the created IRenderPipeline.
     */
    std::shared_ptr<Interstellar::Graphics::IRenderPipeline> OpenGLGraphics::CreatePipeline(std::shared_ptr<Interstellar::Graphics::IShader> shader) {
        return std::make_shared<OpenGLPipeline>(shader);
    }

    /**
     * @brief Submits a mesh to be drawn using the given pipeline.
     *
     * This binds the shader, sets up texture unit 0 (if applicable),
     * binds the mesh, and issues the draw call.
     *
     * @param mesh Mesh to render.
     * @param pipeline Associated render pipeline.
     */
    void OpenGLGraphics::SubmitMesh(
        std::shared_ptr<Interstellar::Graphics::IMesh> mesh,
        std::shared_ptr<Interstellar::Graphics::IRenderPipeline> pipeline)
    {
        auto glMesh = std::static_pointer_cast<OpenGLMesh>(mesh);
        auto oglPipeline = std::static_pointer_cast<OpenGLPipeline>(pipeline);
        auto glShader = std::static_pointer_cast<OpenGLShader>(pipeline->GetShader());

        // 1) Grab the native handle (void*)...
        void* nativeHandle = glShader->GetNativeHandle();

        // 2) Reinterpret it as an integer type guaranteed to hold a pointer
        uintptr_t handleInt = reinterpret_cast<uintptr_t>(nativeHandle);

        // 3) Then downcast to GLuint safely
        GLuint programID = static_cast<GLuint>(handleInt);

        glUseProgram(programID);

        // Set texture sampler uniform (texture unit 0)
        GLint textureUniformLoc = glGetUniformLocation(programID, "u_Texture");
        if (textureUniformLoc >= 0) {
            glUniform1i(textureUniformLoc, 0);
        }
        else {
            logGraphic.LogWarn("Shader '{}' is missing uniform 'u_Texture'.", glShader->GetName());
        }

        glMesh->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(glMesh->GetIndexCount()), GL_UNSIGNED_INT, 0);
    }

    void OpenGLGraphics::SubmitMesh(std::shared_ptr<IMesh> mesh,
        std::shared_ptr<IRenderPipeline> pipeline,
        std::shared_ptr<IMaterial> material)
    {
        auto shader = pipeline ? pipeline->GetShader() : nullptr;
        GLuint prog = static_cast<GLuint>(reinterpret_cast<uintptr_t>(shader ? shader->GetNativeHandle() : nullptr));
        glUseProgram(prog);

        // NOTE: qualify the type with the correct namespace:
        if (auto fb = std::dynamic_pointer_cast<Interstellar::Graphics::FallbackMaterial>(material)) {
            // constants -> uniforms
            for (const auto& [name, bytes] : fb->GetConstants()) {
                GLint loc = glGetUniformLocation(prog, name.c_str());
                if (loc < 0) continue;
                switch (bytes.size()) {
                case 4:  glUniform1fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // float
                case 8:  glUniform2fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // vec2
                case 12: glUniform3fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break; // vec3
                case 16: glUniform4fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break; // vec4
                case 64: glUniformMatrix4fv(loc, 1, GL_FALSE, reinterpret_cast<const float*>(bytes.data())); break; // mat4
                default: break;
                }
            }

            // textures -> GL units 0..N
            GLint texUnit = 0;
            for (const auto& [name, tex] : fb->GetTextures()) {
                GLuint gltex = static_cast<GLuint>(reinterpret_cast<uintptr_t>(tex ? tex->GetNativeHandle() : nullptr));
                if (!gltex) continue;
                glActiveTexture(GL_TEXTURE0 + texUnit);
                glBindTexture(GL_TEXTURE_2D, gltex);
                if (GLint samplerLoc = glGetUniformLocation(prog, name.c_str()); samplerLoc >= 0) {
                    glUniform1i(samplerLoc, texUnit);
                }
                ++texUnit;
            }
        }

        // Draw (bind VAO + issue glDraw*)
        SubmitMesh(std::move(mesh), std::move(pipeline)); // reuse legacy VAO/draw path
    }

    /**
     * @brief Checks whether the GLFW window should close.
     * @return true if the window is closing.
     */
    bool OpenGLGraphics::ShouldClose() const {
        return glfwWindowShouldClose(m_Window);
    }

    /**
     * @brief Returns the native window handle.
     * @return Pointer to the GLFW window.
     */
    void* OpenGLGraphics::GetNativeWindow() const {
        return static_cast<void*>(m_Window);
    }
}