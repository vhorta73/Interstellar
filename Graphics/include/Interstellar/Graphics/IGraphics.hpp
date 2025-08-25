#pragma once
#include <cstdint>
#include <cstddef>   // size_t
#include <memory>
#include <string>

namespace Interstellar::Graphics {

    class IMesh;
    class ITexture;
    class IShader;
    class IRenderPipeline;
    class IMaterial;

    /**
     * @file
     * @ingroup Graphics
     * @brief Public graphics abstraction interfaces (backend-agnostic).
     * @details
     *   Defines the platform-agnostic rendering facade used by the engine.
     *   Concrete backends (OpenGL, Vulkan, DirectX12, Metal) implement this API.
     */

     /**
      * @brief Supported graphics APIs for backend rendering.
      * @ingroup Graphics
      * @since 1.0
      */
    enum class GraphicsAPI {
        DirectX12,  ///< DirectX 12-based renderer. @ingroup Graphics
        Metal,      ///< Metal-based renderer (Apple platforms). @ingroup Graphics
        OpenGL,     ///< OpenGL-based renderer. @ingroup Graphics
        Vulkan,     ///< Vulkan-based renderer. @ingroup Graphics
        Unknown     ///< Fallback or uninitialised API state. @ingroup Graphics
    };

    /**
     * @brief Interface for platform-agnostic graphics API abstraction.
     * @ingroup Graphics
     * @details
     *   This interface provides methods to initialise the renderer, create graphics
     *   resources, submit them for rendering, and manage the frame loop.
     * @thread_safety Unless otherwise stated, all methods must be called on the render/main thread.
     *                Implementations are not required to be thread-safe.
     * @since 1.0
     */
    class IGraphics {
    public:
        virtual ~IGraphics() = default;

        /**
         * @brief Initialise the graphics subsystem with a window of the given size.
         * @ingroup Graphics
         * @param width  Window width in pixels.
         * @param height Window height in pixels.
         * @param vsync  Enable vertical sync if true.
         * @return true on success; false otherwise.
         * @pre Not already initialised.
         * @post On success, a native window/context exists and GetAPI() is valid.
         * @warning Implementations may allocate GPU/CPU resources and create OS handles.
         * @see Shutdown()
         * @see Resize()
         * @since 1.0
         */
        virtual bool Initialise(uint32_t width, uint32_t height, bool vsync) = 0;

        /**
         * @brief Shut down the graphics subsystem and release resources.
         * @ingroup Graphics
         * @details Releases GPU resources, destroys the graphics context, and closes the window.
         * @pre Previously initialised.
         * @post Subsequent rendering calls are invalid until Initialise() is called again.
         * @see Initialise()
         * @see EndFrame()
         * @since 1.0
         */
        virtual void Shutdown() = 0;

        /**
         * @brief Prepare the graphics context for a new frame.
         * @ingroup Graphics
         * @details Clears the frame targets and sets up state for rendering.
         * @pre Initialise() succeeded and the window has not been closed.
         * @post SubmitMesh() may be called for this frame.
         * @see EndFrame()
         * @see SubmitMesh()
         * @since 1.0
         */
        virtual void BeginFrame() = 0;

        /**
         * @brief Finalise the current frame and present it.
         * @ingroup Graphics
         * @details Flushes pending commands and presents to the display. May block
         *          if vsync is enabled or the GPU queue is full.
         * @pre BeginFrame() has been called for this frame.
         * @post Rendering commands for the frame are complete.
         * @see BeginFrame()
         * @see SubmitMesh()
         * @since 1.0
         */
        virtual void EndFrame() = 0;

        /**
         * @brief Handle a window resize.
         * @ingroup Graphics
         * @param width  New width in pixels.
         * @param height New height in pixels.
         * @details Recreates or resizes swapchain/framebuffer targets as needed.
         * @pre Initialise() succeeded.
         * @post Subsequent frames render at the new size.
         * @see Initialise()
         * @since 1.0
         */
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        /**
         * @brief Create a mesh from vertex and index data.
         * @ingroup Graphics
         * @param vertexData Pointer to contiguous vertex bytes (must remain valid until call returns).
         * @param vertexSize Size of vertexData in bytes.
         * @param indexData  Pointer to contiguous index bytes (must remain valid until call returns).
         * @param indexSize  Size of indexData in bytes.
         * @return Shared pointer to IMesh on success; otherwise may return nullptr.
         * @pre vertexData != nullptr when vertexSize > 0; indexData != nullptr when indexSize > 0.
         * @post Returned IMesh retains any necessary GPU copies; input buffers need not outlive the mesh.
         * @warning Exact vertex/index layout is backend/material dependent.
         * @see SubmitMesh()
         * @since 1.0
         */
        [[nodiscard]] virtual std::shared_ptr<IMesh>
            CreateMesh(const void* vertexData, size_t vertexSize,
                const void* indexData, size_t indexSize) = 0;

        /**
         * @brief Create a texture from a file path.
         * @ingroup Graphics
         * @param path Filesystem path to the texture asset.
         * @return Shared pointer to ITexture on success; otherwise may return nullptr.
         * @warning Supported formats are backend/toolchain specific.
         * @since 1.0
         */
        [[nodiscard]] virtual std::shared_ptr<ITexture>
            CreateTexture(const std::string& path) = 0;

        /**
         * @brief Create a shader from a file or identifier.
         * @ingroup Graphics
         * @param path Base name or identifier used to locate shader sources/binaries.
         * @return Shared pointer to IShader on success; otherwise may return nullptr.
         * @note Implementations may expect multiple stage files derived from the base name.
         * @see CreatePipeline()
         * @since 1.0
         */
        [[nodiscard]] virtual std::shared_ptr<IShader>
            CreateShader(const std::string& path) = 0;

        /**
         * @brief Create a render pipeline using the provided shader.
         * @ingroup Graphics
         * @param shader Shader to attach to the pipeline (must be non-null).
         * @return Shared pointer to IRenderPipeline on success; otherwise may return nullptr.
         * @pre shader != nullptr.
         * @see IRenderPipeline
         * @see CreateShader()
         * @see SubmitMesh()
         * @since 1.0
         */
        [[nodiscard]] virtual std::shared_ptr<IRenderPipeline>
            CreatePipeline(std::shared_ptr<IShader> shader) = 0;

        /**
         * @brief Submit a mesh for rendering with a pipeline.
         * @ingroup Graphics
         * @param mesh     Mesh to draw (created by CreateMesh()).
         * @param pipeline Pipeline/shader state to use (created by CreatePipeline()).
         * @pre BeginFrame() has been called for the current frame.
         * @pre mesh != nullptr and pipeline != nullptr.
         * @since 1.0
         */
        virtual void SubmitMesh(std::shared_ptr<IMesh> mesh,
            std::shared_ptr<IRenderPipeline> pipeline) = 0;

        // NEW overload with a safe default that falls back to the old one.
        // This makes current backends & tests compile without implementing it.
        virtual void SubmitMesh(std::shared_ptr<IMesh> mesh,
            std::shared_ptr<IRenderPipeline> pipeline,
            std::shared_ptr<IMaterial> /*material*/) {
            // Default: ignore material until backend supports it.
            SubmitMesh(std::move(mesh), std::move(pipeline));
        }

        /**
         * @brief Get the name of the graphics renderer.
         * @ingroup Graphics
         * @return Human-readable renderer name (for diagnostics).
         * @see GetAPI()
         * @since 1.0
         */
        [[nodiscard]] virtual std::string GetRendererName() const = 0;

        /**
         * @brief Get the active graphics API.
         * @ingroup Graphics
         * @return GraphicsAPI value (for feature checks and telemetry).
         * @see GraphicsAPI
         * @see GetRendererName()
         * @since 1.0
         */
        [[nodiscard]] virtual GraphicsAPI GetAPI() const = 0;

        /**
         * @brief Check if the window should close.
         * @ingroup Graphics
         * @return true if the window/app should terminate; false otherwise.
         * @see Initialise()
         * @see Shutdown()
         * @since 1.0
         */
        [[nodiscard]] virtual bool ShouldClose() const = 0;

        /**
         * @brief Get a pointer to the native window handle (opaque).
         * @ingroup Graphics
         * @return Opaque pointer to the platform window (for example, GLFWwindow*).
         * @note Type and lifetime are backend-specific; treat as read-only.
         * @since 1.0
         */
        [[nodiscard]] virtual void* GetNativeWindow() const = 0;
    };

} // namespace Interstellar::Graphics
