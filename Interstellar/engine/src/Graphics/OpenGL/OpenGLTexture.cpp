#include "OpenGLTexture.hpp"

#include <glad/glad.h>
#include <stb_image.h>
#include "Interstellar/Core/Logging.hpp"

using namespace Interstellar::Graphics::OpenGL;
static const Interstellar::Core::Logger s_Logger(Interstellar::Core::LOG_GRAPHIC);

OpenGLTexture::OpenGLTexture(const std::string& path)
    : m_Name(path)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true); // Flip image for OpenGL convention
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!data) {
        s_Logger.LogError("Failed to load texture from path: {}", path);
        return;
    }

    m_Width = static_cast<uint32_t>(width);
    m_Height = static_cast<uint32_t>(height);

    GLenum format;
    if (channels == 4) {
        format = GL_RGBA;
        m_Format = "RGBA";
    }
    else if (channels == 3) {
        format = GL_RGB;
        m_Format = "RGB";
    }
    else if (channels == 1) {
        format = GL_RED;
        m_Format = "RED";
    }
    else {
        format = GL_RGB;
        m_Format = "Unknown";
    }

    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Default texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    s_Logger.LogInfo("Loaded texture '{}': {}x{}, format = {}", path, m_Width, m_Height, m_Format);
}

OpenGLTexture::~OpenGLTexture() {
    if (m_TextureID) {
        glDeleteTextures(1, &m_TextureID);
        m_TextureID = 0;
    }
}

uint32_t OpenGLTexture::GetWidth() const {
    return m_Width;
}

uint32_t OpenGLTexture::GetHeight() const {
    return m_Height;
}

std::string OpenGLTexture::GetFormat() const {
    return m_Format;
}

const std::string& OpenGLTexture::GetName() const {
    return m_Name;
}

void* OpenGLTexture::GetNativeHandle() const {
    return reinterpret_cast<void*>(static_cast<uintptr_t>(m_TextureID));
}
