#pragma once

#include "Interstellar/Graphics/Core/IGraphics.hpp"

struct GLFWwindow; // Forward declare to avoid including GLFW in header.

namespace Interstellar::Graphics::OpenGL {

    /**
     * @class OpenGLGraphics
     * @brief Concrete implementation of the IGraphics interface using OpenGL and GLFW.
     *
     * This class encapsulates the OpenGL rendering backend logic,
     * managing the window context, frame lifecycle, and creation of core GPU resources.
     *
     * Internally relies on GLFW for window and context management.
     *
     * @since 1.0
     */
    class OpenGLGraphics : public Core::IGraphics {
    public:
        /**
         * @brief Constructs an OpenGLGraphics instance.
         * @since 1.0
         */
        explicit OpenGLGraphics();

        /**
         * @brief Destructor that ensures GPU cleanup and window shutdown.
         * @since 1.0
         */
        ~OpenGLGraphics() override;

        /**
         * @brief Initialises the OpenGL context and GLFW window.
         *
         * @param width Initial width of the rendering window.
         * @param height Initial height of the rendering window.
         * @param vsync Enables or disables vertical sync.
         * @return True if initialisation succeeded.
         * @since 1.0
         * @see Shutdown()
         */
        bool Initialise(uint32_t width, uint32_t height, bool vsync) override;

        /**
         * @brief Shuts down the OpenGL context and releases all resources.
         * @since 1.0
         * @see Initialise()
         */
        void Shutdown() override;

        /**
         * @brief Begins rendering a new frame.
         * Clears buffers and prepares OpenGL state.
         *
         * @since 1.0
         * @see EndFrame()
         */
        void BeginFrame() override;

        /**
         * @brief Ends the current frame and swaps buffers.
         * @since 1.0
         * @see BeginFrame()
         */
        void EndFrame() override;

        /**
         * @brief Resizes the window and adjusts OpenGL viewport.
         *
         * @param width New width of the window.
         * @param height New height of the window.
         * @since 1.0
         */
        void Resize(uint32_t width, uint32_t height) override;

        /**
         * @brief Creates a mesh object using OpenGL buffers.
         *
         * @param vertexData Pointer to vertex data.
         * @param vertexSize Size of vertex buffer in bytes.
         * @param indexData Pointer to index data.
         * @param indexSize Size of index buffer in bytes.
         * @return A shared pointer to a mesh object.
         * @since 1.0
         * @see SubmitMesh()
         */
        [[nodiscard]] std::shared_ptr<Core::IMesh> CreateMesh(
            const void* vertexData, size_t vertexSize,
            const void* indexData, size_t indexSize
        ) override;

        /**
         * @brief Loads a texture from file.
         *
         * @param path Path to the image file.
         * @return A shared pointer to the created texture.
         * @since 1.0
         */
        [[nodiscard]] std::shared_ptr<Core::ITexture> CreateTexture(const std::string& path) override;

        /**
         * @brief Loads and compiles a shader program.
         *
         * @param name Logical name or file path identifier.
         * @return A shared pointer to the shader.
         * @since 1.0
         */
        [[nodiscard]] std::shared_ptr<Core::IShader> CreateShader(const std::string& name) override;

        /**
         * @brief Creates a render pipeline based on the given shader.
         *
         * @param shader Shader to attach to the pipeline.
         * @return A shared pointer to the pipeline object.
         * @since 1.0
         */
        [[nodiscard]] std::shared_ptr<Core::IRenderPipeline> CreatePipeline(std::shared_ptr<Core::IShader> shader) override;

        /**
         * @brief Submits a mesh to the renderer using a given pipeline.
         *
         * @param mesh Mesh to render.
         * @param pipeline Pipeline configuration to use.
         * @since 1.0
         * @see CreateMesh()
         * @see BeginFrame()
         * @see EndFrame()
         */
        void SubmitMesh(
            std::shared_ptr<Core::IMesh> mesh,
            std::shared_ptr<Core::IRenderPipeline> pipeline
        ) override;

        /**
         * @brief Gets the renderer name.
         *
         * @return A string identifying the renderer, e.g., "OpenGL".
         * @since 1.0
         * @see GetAPI()
         */
        [[nodiscard]] std::string GetRendererName() const override;

        /**
         * @brief Returns the graphics API in use (OpenGL).
         *
         * @return The GraphicsAPI enum value.
         * @since 1.0
         */
        [[nodiscard]] Core::GraphicsAPI GetAPI() const override;

        /**
         * @brief Checks whether the window should close.
         *
         * @return True if the window is requesting to close.
         * @since 1.0
         */
        [[nodiscard]] bool ShouldClose() const override;

    private:
        GLFWwindow* m_Window = nullptr;
        bool m_Vsync = true;
    };

}
