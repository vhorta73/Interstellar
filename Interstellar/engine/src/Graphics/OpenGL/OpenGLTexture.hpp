#pragma once

#include "Interstellar/Graphics/Core/ITexture.hpp"
#include <string>

namespace Interstellar::Graphics::OpenGL {

    /// Represents an OpenGL texture loaded from an image file.
    class OpenGLTexture : public Core::ITexture {
    public:
        /// Constructs a texture by loading the image from disk.
        /// @param path Path to the image file.
        explicit OpenGLTexture(const std::string& path);

        /// Releases the texture from GPU memory.
        ~OpenGLTexture();

        /// Returns the width of the texture in pixels.
        uint32_t GetWidth() const override;

        /// Returns the height of the texture in pixels.
        uint32_t GetHeight() const override;

        /// Returns the format of the texture (e.g., "RGBA").
        std::string GetFormat() const override;

        /// Returns the name of the texture (usually the filename).
        const std::string& GetName() const override;

        /// Returns the OpenGL texture ID.
        void* GetNativeHandle() const override;

    private:
        unsigned int m_TextureID = 0;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        std::string m_Name;
        std::string m_Format = "Unknown";
    };

}
