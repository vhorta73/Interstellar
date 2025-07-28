#pragma once

#include <cstdint>
#include <string>

namespace Interstellar::Graphics::Core {

    /**
     * @brief Interface for a platform-agnostic GPU texture resource.
     *
     * Represents a 2D texture loaded from disk or created dynamically at runtime.
     * This interface provides access to essential metadata, such as dimensions and format,
     * as well as integration with underlying graphics API handles.
     *
     * Textures may be used for rendering, sampling in shaders, or as attachments in framebuffers.
     *
     * @since 1.0
     */
    class ITexture {
    public:
        virtual ~ITexture() = default;

        /**
         * @brief Gets the width of the texture in pixels.
         *
         * @return Width in pixels.
         *
         * @since 1.0
         */
        virtual uint32_t GetWidth() const = 0;

        /**
         * @brief Gets the height of the texture in pixels.
         *
         * @return Height in pixels.
         *
         * @since 1.0
         */
        virtual uint32_t GetHeight() const = 0;

        /**
         * @brief Returns the internal format of the texture.
         *
         * The format is returned as a string for flexibility (e.g., `"RGBA8"`, `"BC7"`, `"R16F"`).
         * This allows custom formats or platform-specific representations to be surfaced without strict enums.
         *
         * @return A string describing the pixel format.
         *
         * @since 1.0
         */
        virtual std::string GetFormat() const = 0;

        /**
         * @brief Retrieves the debug or symbolic name assigned to this texture.
         *
         * Useful for logging, UI overlays, asset inspection, or GPU debugging.
         *
         * @return A constant reference to the name string.
         *
         * @since 1.0
         */
        virtual const std::string& GetName() const = 0;

        /**
         * @brief Returns the backend-specific texture handle for low-level API access.
         *
         * The returned handle may represent:
         * - OpenGL: GLuint (texture ID)
         * - Vulkan: VkImage or descriptor view
         * - DirectX: ID3D12Resource or ID3D11ShaderResourceView
         *
         * Treat this handle as opaque unless you are interfacing directly with the native API.
         *
         * @return A void pointer to the native texture handle.
         *
         * @since 1.0
         */
        virtual void* GetNativeHandle() const = 0;
    };
}
