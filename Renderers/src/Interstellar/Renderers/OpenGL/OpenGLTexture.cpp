#include "Interstellar/Renderers/OpenGL/OpenGLTexture.hpp"
#include "Interstellar/Logging/Logging.hpp"

#include <glad/glad.h>
#include <Interstellar/Renderers/OpenGL/stb/stb_image.hpp>

namespace {

    /**
     * @brief Maps number of image channels to OpenGL format and string label.
     *
     * @param channels Number of color channels returned by stb_image.
     * @param formatStr [out] Format name string (e.g., "RGBA").
     * @return GLenum representing the OpenGL texture format.
     */
    GLenum DetermineGLFormat(int channels, std::string& formatStr) {
        switch (channels) {
        case 4: formatStr = "RGBA"; return GL_RGBA;
        case 3: formatStr = "RGB";  return GL_RGB;
        case 1: formatStr = "RED";  return GL_RED;
        default: formatStr = "Unknown"; return GL_RGB;
        }
    }

} // anonymous namespace

namespace Interstellar::Renderers::OpenGL {

    using namespace Interstellar::Logging;

    /**
     * @brief Constructs an OpenGL texture by loading an image from file.
     *
     * Loads the image using stb_image, generates a GL texture, sets default
     * sampling parameters, and uploads the data to the GPU.
     *
     * @param path File path to the image.
     */
    OpenGLTexture::OpenGLTexture(const std::string& path)
        : m_Name(path)
    {
        int width = 0, height = 0, channels = 0;
        stbi_set_flip_vertically_on_load(true); // OpenGL expects bottom-left origin
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (!data) {
            logGraphic.LogError("Failed to load texture from path: {}", path);
            return;
        }

        m_Width = static_cast<uint32_t>(width);
        m_Height = static_cast<uint32_t>(height);

        const GLenum format = DetermineGLFormat(channels, m_Format);
        if (m_Format == "Unknown") {
            logGraphic.LogWarn("Texture '{}' loaded with unsupported channel count: {}", path, channels);
        }

        constexpr GLenum target = GL_TEXTURE_2D;

        glGenTextures(1, &m_TextureID);
        glBindTexture(target, m_TextureID);

        glTexImage2D(target, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);

        if (m_Width > 1 && m_Height > 1) {
            glGenerateMipmap(target);
        }

        // Default texture sampling and wrapping parameters
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(target, 0);
        stbi_image_free(data);

        logGraphic.LogInfo("Loaded texture '{}': {}x{}, format = {}", m_Name, m_Width, m_Height, m_Format);
    }

    /**
     * @brief Destroys the texture and releases GPU memory.
     */
    OpenGLTexture::~OpenGLTexture() {
        if (m_TextureID) {
            glDeleteTextures(1, &m_TextureID);
            m_TextureID = 0;
        }
    }

    /**
     * @brief Returns the OpenGL texture target.
     *
     * This implementation always returns `GL_TEXTURE_2D`.
     *
     * @return GLenum texture target used internally.
     */
    GLenum OpenGLTexture::GetTextureTarget() const {
        return GL_TEXTURE_2D;
    }

    /**
     * @brief Gets the texture width in pixels.
     * @return Width in pixels.
     */
    uint32_t OpenGLTexture::GetWidth() const {
        return m_Width;
    }

    /**
     * @brief Gets the texture height in pixels.
     * @return Height in pixels.
     */
    uint32_t OpenGLTexture::GetHeight() const {
        return m_Height;
    }

    /**
     * @brief Gets the internal format of the texture as a string.
     * @return Format string (e.g., "RGBA", "RGB", etc.)
     */
    std::string OpenGLTexture::GetFormat() const {
        return m_Format;
    }

    /**
     * @brief Gets the symbolic or file-based name of the texture.
     * @return Reference to the internal name string.
     */
    const std::string& OpenGLTexture::GetName() const {
        return m_Name;
    }

    /**
     * @brief Returns the native OpenGL texture ID.
     *
     * The pointer can be cast back to `GLuint` when necessary.
     *
     * @return Opaque pointer representing the GL texture.
     */
    void* OpenGLTexture::GetNativeHandle() const {
        return reinterpret_cast<void*>(static_cast<uintptr_t>(m_TextureID));
    }
}