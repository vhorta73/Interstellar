#pragma once

#include "Interstellar/Graphics/Core/ITexture.hpp"
#include <string>

namespace Interstellar::Graphics::OpenGL {

    /**
     * @class OpenGLTexture
     * @brief Represents an OpenGL texture loaded from an image file.
     *
     * This class implements the `ITexture` interface and handles texture loading, format parsing,
     * and OpenGL-specific resource management (e.g., glGenTextures, glBindTexture).
     *
     * It exposes metadata such as width, height, and format for rendering or UI purposes.
     *
     * @since 1.0
     */
    class OpenGLTexture : public Core::ITexture {
    public:
        /**
         * @brief Constructs a texture by loading the image from disk.
         *
         * The texture is uploaded to the GPU and an OpenGL texture ID is generated.
         *
         * @param path Path to the image file.
         * @since 1.0
         */
        explicit OpenGLTexture(const std::string& path);

        /**
         * @brief Destructor. Releases OpenGL texture resources from GPU memory.
         * @since 1.0
         */
        ~OpenGLTexture() override;

        /**
         * @brief Returns the width of the texture in pixels.
         *
         * @return Texture width in pixels.
         * @since 1.0
         */
        [[nodiscard]] uint32_t GetWidth() const override;

        /**
         * @brief Returns the height of the texture in pixels.
         *
         * @return Texture height in pixels.
         * @since 1.0
         */
        [[nodiscard]] uint32_t GetHeight() const override;

        /**
         * @brief Returns the format of the texture as a string (e.g., "RGBA8", "BC7").
         *
         * Format is either inferred during loading or set to "Unknown" if not applicable.
         *
         * @return A string representing the texture format.
         * @since 1.0
         */
        [[nodiscard]] std::string GetFormat() const override;

        /**
         * @brief Returns the name of the texture (typically the file name).
         *
         * May be used for debugging, asset introspection, or UI display.
         *
         * @return A string reference representing the name.
         * @since 1.0
         */
        [[nodiscard]] const std::string& GetName() const override;

        /**
         * @brief Returns the native OpenGL texture ID.
         *
         * Exposed as a void pointer to maintain backend-agnostic API contracts.
         * Can be cast to `GLuint*` for OpenGL-specific usage.
         *
         * @return A pointer to the OpenGL texture ID.
         * @since 1.0
         */
        [[nodiscard]] void* GetNativeHandle() const override;

    private:
        unsigned int m_TextureID = 0;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        std::string m_Name;
        std::string m_Format = "Unknown";
    };

}
