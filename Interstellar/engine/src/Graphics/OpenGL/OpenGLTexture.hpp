#pragma once

#include "Interstellar/Graphics/Core/ITexture.hpp"
#include <string>

namespace Interstellar::Graphics::OpenGL {

    class OpenGLTexture : public Core::ITexture {
    public:
        OpenGLTexture(const std::string& path);
        ~OpenGLTexture();

        uint32_t GetWidth() const override;
        uint32_t GetHeight() const override;
        std::string GetFormat() const override;
        const std::string& GetName() const override;
        void* GetNativeHandle() const override;

    private:
        unsigned int m_TextureID = 0;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        std::string m_Name;
    };

}
