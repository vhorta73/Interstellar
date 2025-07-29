#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace Interstellar::Graphics::Core {

    class IMesh;
    class ITexture;
    class IShader;
    class IRenderPipeline;

    /**
    * @brief Supported graphics APIs for backend rendering.
    * 
    * @since 1.0
    */
    enum class GraphicsAPI {
        DirectX12,  ///< DirectX 12-based renderer
        Metal,      ///< Metal-based renderer (for Apple platforms)
        OpenGL,     ///< OpenGL-based renderer
        Vulkan,     ///< Vulkan-based renderer
        Unknown     ///< Fallback or uninitialised API state
    };

    /**
    * @brief Interface for platform-agnostic graphics API abstraction.
    *
    * This interface provides methods to initialise the renderer, create graphics
    * resources, submit them for rendering, and manage the render loop.
    * Concrete implementations may use @a OpenGL, Vulkan, DirectX, etc.
    * 
    * @since 1.0
    */
    class IGraphics {
    public:
        virtual ~IGraphics() = default;

        /**
        * @brief Initialises the graphics subsystem with a window of the given size.
        *
        * @param width Width of the window in pixels.
        * @param height Height of the window in pixels.
        * @param vsync Whether to enable vertical sync.
        * 
        * @return true if initialisation succeeded.
        *
        * @since 1.0
        * @see Shutdown()
        * @see Resize()
        */
        virtual bool Initialise(uint32_t width, uint32_t height, bool vsync) = 0;

        /**
        * @brief Shuts down the graphics subsystem and cleans up resources.
        * 
        * This method is typically called in the application shutdown sequence.
        * It is important to call this method to avoid memory leaks and
        * ensure that all graphics resources are properly released.
        * Releasing GPU resources, destroying the graphics context,
        * and closing the rendering window.
        * 
        * @since 1.0
        * @see Initialise()
        * @see EndFrame()
        */
        virtual void Shutdown() = 0;

        /**
        * @brief Prepares the graphics context for a new frame.
        * 
        * Clears the screen, resets GPU state, and sets up the viewport.
        * Should be called once per frame before rendering any content.
        * 
        * @since 1.0
        * @see EndFrame()
        * @see SubmitMesh()
        */
        virtual void BeginFrame() = 0;

        /**
        * @brief Ends the current frame and submits it for display.
        * 
        * Called at the end of each frame to finalize the rendering operations.
        * Implementations should ensure that all rendering commands
        * are completed before this call, as it may block until the frame is fully rendered.
        * 
        * @since 1.0
        * @see BeginFrame()
        * @see SubmitMesh()
        */
        virtual void EndFrame() = 0;

        /**
        * @brief Resizes the rendering window.
        * 
        * Called when the window is resized to update the graphics context accordingly.
        * Implementations should ensure that the graphics context
        * is properly updated to reflect the new window size
        * and that rendering operations can continue without issues
        * after the resize.
        * 
        * This method is typically called in response to window resize events,
        * such as when the user resizes the application window
        * or when the display resolution changes.
        *
        * @param width New width of the window in pixels.
        * @param height New height of the window in pixels.
        * 
        * @since 1.0
        * @see Initialise()
        */
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        /**
        * @brief Creates a mesh from vertex and index data.
        * 
        * @param vertexData Pointer to the vertex data.
        * @param vertexSize Size of the vertex data in bytes.
        * @param indexData Pointer to the index data.
        * @param indexSize Size of the index data in bytes.
        * 
        * @return A shared pointer to an @c IMesh instance.
        *
        * @since 1.0
        * @see SubmitMesh()
        * @see @c IMesh
        */
        [[nodiscard]] virtual std::shared_ptr<IMesh> CreateMesh(const void* vertexData, size_t vertexSize,
            const void* indexData, size_t indexSize) = 0;

        /**
        * @brief Creates a texture from a file path.
        * 
        * @param path Path to the texture file.
        * 
        * @return A shared pointer to an @c ITexture instance.
        *
        * @since 1.0
        * @see @c ITexture
        */
        [[nodiscard]] virtual std::shared_ptr<ITexture> CreateTexture(const std::string& path) = 0;

        /**
        * @brief Creates a shader from a file path.
        * 
        * @param path Base name or identifier used to locate vertex/fragment shader source files.
        * 
        * @return A shared pointer to an @c IShader instance.
        *
        * @since 1.0
        * @see @c IShader
        * @see CreatePipeline()
        */
        [[nodiscard]] virtual std::shared_ptr<IShader> CreateShader(const std::string& path) = 0;

        /**
        * @brief Creates a render pipeline using the provided shader.
        * 
        * @param shader A shared pointer to an @c IShader to be used by the render pipeline.
        * 
        * @return A shared pointer to an @IRenderPipeline instance.
        *
        * @since 1.0
        * @see @c IRenderPipeline
        * @see CreateShader()
        * @see SubmitMesh()
        */
        [[nodiscard]] virtual std::shared_ptr<IRenderPipeline> CreatePipeline(std::shared_ptr<IShader> shader) = 0;

        /**
        * @brief Submits a mesh for rendering using the specified pipeline.
        * 
        * @param mesh The @c IMesh to render. Must be created with CreateMesh().
        * @param pipeline The @c IRenderPipeline to use for rendering the @c IMesh (includes shader and bindings).
        * 
        * @since 1.0
        * @see CreateMesh()
        * @see CreatePipeline()
        * @see BeginFrame()
        * @see EndFrame()
        */
        virtual void SubmitMesh(std::shared_ptr<IMesh> mesh, std::shared_ptr<IRenderPipeline> pipeline) = 0;

        /**
        * @brief Gets the name of the graphics renderer.
        * 
        * @return The name of the graphics renderer as a string.
        * 
        * @since 1.0
        * @see GetAPI()
        */
        [[nodiscard]] virtual std::string GetRendererName() const = 0;

        /**
        * @brief Gets the version of the graphics API being used.
        * 
        * @return The graphics API in use (e.g., @a Vulkan, @a OpenGL, @a DirectX, etc.).
        *
        * @since 1.0
        * @see @c GraphicsAPI
        * @see GetRendererName()
        */
        [[nodiscard]] virtual GraphicsAPI GetAPI() const = 0;

        /**
        * @brief Checks if the graphics window should close.
        * 
        * @return True if the window should close, false otherwise.
        *
        * @since 1.0
        * @see Initialise()
        * @see Shutdown()
        */
        [[nodiscard]] virtual bool ShouldClose() const = 0;

        /**
        * @brief Returns a pointer to the native window handle (e.g., GLFWwindow*).
        * 
        * @return Pointer to native window (opaque void* to allow cross-platform use).
        * 
        * @since 1.0
        */
        [[nodiscard]] virtual void* GetNativeWindow() const = 0;

    };

}
