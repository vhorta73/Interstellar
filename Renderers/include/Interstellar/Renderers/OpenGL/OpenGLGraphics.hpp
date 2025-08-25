#pragma once

#include <cstdint>   // uint32_t
#include <cstddef>   // size_t
#include <memory>    // std::shared_ptr
#include <string>

#include "Interstellar/Graphics/IGraphics.hpp"

/**
 * @file
 * @ingroup Renderers
 * @brief OpenGL renderer backend: GLFW-based implementation of IGraphics.
 * @details
 *   Concrete renderer that uses OpenGL for drawing and GLFW for window/context
 *   creation. This header declares the public interface; see the .cpp for
 *   details and error handling.
 *
 *   Lifecycle:
 *     1) Construct OpenGLGraphics
 *     2) Initialise()
 *     3) Per frame: BeginFrame() -> SubmitMesh(...) -> EndFrame()
 *     4) Shutdown() before destruction (also called from destructor as a guard)
 *
 *   Threading:
 *     Unless stated otherwise by the implementation, all methods are expected
 *     to be called on the render/main thread.
 *
 *   Native handles:
 *     GetNativeWindow() returns a GLFWwindow* as void*. Treat as opaque unless
 *     you are writing platform-specific code.
 */

 /**
  * @class GLFWwindow
  * @brief Forward declaration of GLFW window type to avoid header dependency.
  */
struct GLFWwindow; // forward declare to avoid including <GLFW/glfw3.h> here

namespace Interstellar::Renderers::OpenGL {

    /**
     * @brief OpenGL + GLFW implementation of Interstellar::Graphics::IGraphics.
     * @ingroup Renderers
     * @details
     *   Manages window/context, frame lifecycle, and creation of core GPU
     *   resources (mesh, texture, shader, pipeline).
     *
     * @since 1.0
     * @see Interstellar::Graphics::IGraphics
     * @see Interstellar::Graphics::IMesh
     * @see Interstellar::Graphics::ITexture
     * @see Interstellar::Graphics::IShader
     * @see Interstellar::Graphics::IRenderPipeline
     */
    class OpenGLGraphics : public Interstellar::Graphics::IGraphics {
    public:
        /**
         * @brief Construct an instance; no GL context is created yet.
         * @since 1.0
         */
        explicit OpenGLGraphics();

        /**
         * @brief Destructor; calls Shutdown() if not already called.
         * @since 1.0
         */
        ~OpenGLGraphics() override;

        /**
         * @brief Create the GLFW window and initialize the OpenGL context.
         * @param width  Initial client width in pixels.
         * @param height Initial client height in pixels.
         * @param vsync  Enable or disable vertical sync.
         * @return true on success; false on failure.
         * @pre Must be called before any other rendering operation.
         * @post On success, GetNativeWindow() returns a valid pointer.
         * @since 1.0
         * @see Shutdown()
         */
        bool Initialise(uint32_t width, uint32_t height, bool vsync) override;

        /**
         * @brief Tear down the context and free GPU resources.
         * @details Safe to call multiple times; subsequent calls are no-ops.
         * @since 1.0
         * @see Initialise()
         */
        void Shutdown() override;

        /**
         * @brief Begin a new frame: clear buffers and prepare state.
         * @since 1.0
         * @pre Initialise() has succeeded and the window is not closing.
         * @see EndFrame()
         * @see SubmitMesh()
         */
        void BeginFrame() override;

        /**
         * @brief End the frame and present: flush, swap buffers, poll events.
         * @since 1.0
         * @pre A matching BeginFrame() has occurred this frame.
         * @see BeginFrame()
         */
        void EndFrame() override;

        /**
         * @brief Handle window resize and update the GL viewport.
         * @param width  New client width in pixels.
         * @param height New client height in pixels.
         * @since 1.0
         * @pre Initialise() has succeeded.
         */
        void Resize(uint32_t width, uint32_t height) override;

        /**
         * @brief Create a mesh backed by OpenGL buffers.
         * @param vertexData Pointer to vertex data.
         * @param vertexSize Size of vertex buffer in bytes.
         * @param indexData  Pointer to index data (can be null for non-indexed).
         * @param indexSize  Size of index buffer in bytes (0 for non-indexed).
         * @return Shared pointer to IMesh or null on failure.
         * @since 1.0
         * @pre Initialise() has succeeded.
         * @see SubmitMesh()
         */
        [[nodiscard]] std::shared_ptr<Interstellar::Graphics::IMesh> CreateMesh(
            const void* vertexData, size_t vertexSize,
            const void* indexData, size_t indexSize
        ) override;

        /**
         * @brief Load a texture from an image file.
         * @param path Filesystem path to the image.
         * @return Shared pointer to ITexture or null on failure.
         * @since 1.0
         * @pre Initialise() has succeeded.
         */
        [[nodiscard]] std::shared_ptr<Interstellar::Graphics::ITexture>
            CreateTexture(const std::string& path) override;

        /**
         * @brief Load and compile a shader or program.
         * @param name Logical name or base path for shader sources.
         * @return Shared pointer to IShader or null on failure.
         * @warning Current demo implementation may be hardcoded to
         *          assets/shaders/triangle.vert and triangle.frag.
         * @since 1.0
         * @see CreatePipeline()
         */
        [[nodiscard]] std::shared_ptr<Interstellar::Graphics::IShader>
            CreateShader(const std::string& name) override;

        /**
         * @brief Create a render pipeline from a shader.
         * @param shader Shader to use for the pipeline.
         * @return Shared pointer to IRenderPipeline or null on failure.
         * @since 1.0
         * @pre shader is non-null and valid.
         */
        [[nodiscard]] std::shared_ptr<Interstellar::Graphics::IRenderPipeline>
            CreatePipeline(std::shared_ptr<Interstellar::Graphics::IShader> shader) override;

        /**
         * @brief Submit a mesh for drawing using the given pipeline.
         * @param mesh     Mesh to draw.
         * @param pipeline Pipeline to bind (owns shader state and bindings).
         * @since 1.0
         * @pre mesh and pipeline were created by this renderer instance, or are
         *      otherwise compatible with this OpenGL context.
         * @see CreateMesh()
         * @see BeginFrame()
         * @see EndFrame()
         */
        void SubmitMesh(
            std::shared_ptr<Interstellar::Graphics::IMesh> mesh,
            std::shared_ptr<Interstellar::Graphics::IRenderPipeline> pipeline
        ) override;

        void SubmitMesh(std::shared_ptr<Interstellar::Graphics::IMesh> mesh,
            std::shared_ptr<Interstellar::Graphics::IRenderPipeline> pipeline,
            std::shared_ptr<Interstellar::Graphics::IMaterial> material) override;

        /**
         * @brief Query a human-readable renderer name.
         * @return For example: "OpenGL".
         * @since 1.0
         * @see GetAPI()
         */
        [[nodiscard]] std::string GetRendererName() const override;

        /**
         * @brief Return the active graphics API.
         * @return GraphicsAPI::OpenGL.
         * @since 1.0
         */
        [[nodiscard]] Interstellar::Graphics::GraphicsAPI GetAPI() const override;

        /**
         * @brief Check whether the window should close.
         * @return true if the user or OS requested close; false otherwise.
         * @since 1.0
         */
        [[nodiscard]] bool ShouldClose() const override;

        /**
         * @brief Get the native window handle (GLFWwindow*).
         * @return Opaque pointer to GLFWwindow. May be null if not initialized.
         * @since 1.0
         */
        [[nodiscard]] void* GetNativeWindow() const override;

    private:
        GLFWwindow* m_Window = nullptr; // native GLFW window pointer (owned by this class)
        bool        m_Vsync = true;    // tracks whether vsync is enabled
    };

} // namespace Interstellar::Renderers::OpenGL
