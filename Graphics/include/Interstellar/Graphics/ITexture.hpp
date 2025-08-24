#pragma once

#include <cstdint>
#include <string>

namespace Interstellar::Graphics {

    /**
     * @file
     * @ingroup Graphics
     * @brief Backend-agnostic GPU texture interface.
     * @details
     *   Represents a 2D texture resource owned by a graphics backend. Implementations
     *   provide dimensions, format information, an optional debug name, and an
     *   opaque native handle for low-level integration.
     */

     /**
      * @brief Platform-agnostic GPU texture resource.
      * @ingroup Graphics
      * @details
      *   Textures may be used as shader-readable resources, render targets, or
      *   attachments, depending on backend capabilities. Unless stated otherwise,
      *   all methods are expected to be called on the render/main thread.
      * @since 1.0
      */
    class ITexture {
    public:
        virtual ~ITexture() = default;

        /**
         * @brief Get the width of the texture in pixels.
         * @ingroup Graphics
         * @return Unsigned width in pixels.
         * @since 1.0
         */
        virtual uint32_t GetWidth() const = 0;

        /**
         * @brief Get the height of the texture in pixels.
         * @ingroup Graphics
         * @return Unsigned height in pixels.
         * @since 1.0
         */
        virtual uint32_t GetHeight() const = 0;

        /**
         * @brief Get the internal pixel format as a string.
         * @ingroup Graphics
         * @details
         *   Returns a backend-defined token such as "RGBA8", "BC7", or "R16F".
         *   Using a string keeps the API flexible without locking into a fixed enum.
         * @return Format identifier string.
         * @since 1.0
         */
        virtual std::string GetFormat() const = 0;

        /**
         * @brief Get the debug or symbolic name of the texture.
         * @ingroup Graphics
         * @details Useful for logging, UI inspection, and GPU debugging. May be empty
         *          if unset or unsupported by the backend.
         * @return Constant reference to the texture name.
         * @since 1.0
         */
        virtual const std::string& GetName() const = 0;

        /**
         * @brief Get a backend-native texture handle (opaque).
         * @ingroup Graphics
         * @details
         *   The concrete type depends on the backend and should be treated as opaque:
         *   - OpenGL: texture id (cast to void*)
         *   - Vulkan: VkImage or view handle
         *   - DirectX: ID3D12Resource* or ID3D11ShaderResourceView*
         *   May return nullptr if not applicable.
         * @return Opaque pointer to the native texture object.
         * @since 1.0
         */
        virtual void* GetNativeHandle() const = 0;
    };

} // namespace Interstellar::Graphics
