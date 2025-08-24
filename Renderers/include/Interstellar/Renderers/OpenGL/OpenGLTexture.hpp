#pragma once

#include <string>
#include <cstdint>

#include <glad/glad.h>                 // for GLenum
#include "Interstellar/Graphics/ITexture.hpp"

/**
 * @file
 * @ingroup Renderers
 * @brief OpenGL texture implementation header.
 * @details
 *   Concrete ITexture that loads an image from disk, creates an OpenGL texture,
 *   and exposes width/height/format and a backend-native handle.
 *
 *   Threading:
 *   - Unless otherwise documented in the .cpp, call methods on the render/main
 *     thread while a valid OpenGL context is current.
 *
 *   Lifetime:
 *   - The OpenGL texture object is owned by this class and is destroyed in the
 *     destructor. Do not retain or use the native handle after destruction.
 */

namespace Interstellar::Renderers::OpenGL {

    /**
     * @brief OpenGL 2D texture (loaded from an image file).
     * @ingroup GraphicsOpenGL
     * @details
     *   Implements ITexture on top of OpenGL. Loads pixels from disk (format
     *   detection is implementation-defined), uploads to GPU, and manages the
     *   GL texture object lifecycle.
     *
     *   Notes:
     *   - GetNativeHandle() returns an opaque token representing the GL texture ID.
     *     Treat it as backend-only. Do not dereference; casting to a numeric type
     *     is only valid in backend code that understands the API.
     *
     * @since 1.0
     * @see Interstellar::Graphics::ITexture
     */
    class OpenGLTexture : public Interstellar::Graphics::ITexture {
    public:
        /**
         * @brief Load an image from disk and create a GPU texture.
         * @param path Path to the image file.
         * @pre An OpenGL context is current on the calling thread.
         * @since 1.0
         */
        explicit OpenGLTexture(const std::string& path);

        /**
         * @brief Release the GL texture object.
         * @since 1.0
         */
        ~OpenGLTexture() override;

        /**
         * @brief OpenGL texture target for this resource.
         * @return GL_TEXTURE_2D or other OpenGL target used internally.
         * @since 1.0
         */
        [[nodiscard]] GLenum GetTextureTarget() const;

        /**
         * @brief Texture width in pixels.
         * @since 1.0
         */
        [[nodiscard]] uint32_t GetWidth() const override;

        /**
         * @brief Texture height in pixels.
         * @since 1.0
         */
        [[nodiscard]] uint32_t GetHeight() const override;

        /**
         * @brief Pixel format description (for example "RGBA8", "BC7").
         * @details String is implementation-defined and may be "Unknown".
         * @since 1.0
         */
        [[nodiscard]] std::string GetFormat() const override;

        /**
         * @brief Debug or asset name (typically the file name).
         * @since 1.0
         */
        [[nodiscard]] const std::string& GetName() const override;

        /**
         * @brief Backend-native handle (opaque).
         * @details
         *   For OpenGL, represents the GLuint texture name encoded as an opaque
         *   pointer token. Do not dereference outside backend code.
         * @return Opaque pointer token for the GL texture.
         * @since 1.0
         */
        [[nodiscard]] void* GetNativeHandle() const override;

    private:
        unsigned int m_TextureID = 0;  // GL texture name (GLuint)
        uint32_t     m_Width = 0;
        uint32_t     m_Height = 0;
        std::string  m_Name;
        std::string  m_Format = "Unknown";
    };

} // namespace Interstellar::Renderers::OpenGL
