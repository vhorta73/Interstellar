#pragma once

#include <string>
#include <vector>

#include "Interstellar/Graphics/IShader.hpp"

/**
 * @file
 * @ingroup Renderers
 * @brief OpenGL shader program implementation header.
 * @details
 *   Concrete IShader backed by an OpenGL program composed of vertex/fragment
 *   stages. Exposes name, stage list, validity, and an opaque native handle.
 *
 *   Threading:
 *   - Unless otherwise documented in the .cpp, call methods on the render/main
 *     thread while a valid OpenGL context is current.
 */

 /**
  * @defgroup GraphicsOpenGL OpenGL Backend
  * @ingroup Renderers
  * @brief Types provided by the OpenGL renderer.
  * @since 1.0
  */

namespace Interstellar::Renderers::OpenGL {

    /**
     * @brief OpenGL shader program (vertex + fragment).
     * @ingroup GraphicsOpenGL
     * @details
     *   Compiles and links an OpenGL program from provided source files and
     *   implements the platform-agnostic IShader interface.
     *
     *   Notes:
     *   - GetNativeHandle() returns an opaque token representing the GL program.
     *     Treat it as a backend-only detail. Do not dereference or rely on its
     *     concrete type outside the backend.
     *
     * @since 1.0
     * @see Interstellar::Graphics::IShader
     */
    class OpenGLShader : public Interstellar::Graphics::IShader {
    public:
        /**
         * @brief Construct from vertex and fragment shader source paths.
         * @param name         Debug/display name.
         * @param vertexPath   Filesystem path to vertex shader source.
         * @param fragmentPath Filesystem path to fragment shader source.
         * @pre A valid OpenGL context is current on the calling thread.
         * @since 1.0
         */
        OpenGLShader(const std::string& name,
            const std::string& vertexPath,
            const std::string& fragmentPath);

        /**
         * @brief Release GL resources (program, attached shaders).
         * @since 1.0
         */
        ~OpenGLShader();

        /**
         * @brief Debug/display name.
         * @return Reference to the shader name.
         * @since 1.0
         */
        [[nodiscard]] const std::string& GetName() const override;

        /**
         * @brief Stages present in this program.
         * @details Typical values: {"vertex","fragment"}.
         * @return Vector of stage names.
         * @since 1.0
         */
        [[nodiscard]] std::vector<std::string> GetAvailableStages() const override;

        /**
         * @brief True if compilation and link succeeded.
         * @return Validity flag.
         * @since 1.0
         */
        [[nodiscard]] bool IsValid() const override;

        /**
         * @brief Backend-native handle (opaque).
         * @details
         *   For OpenGL, this represents the GLuint program ID encoded as an
         *   opaque pointer token. Do not dereference; intended only for backend
         *   interop paths that explicitly understand the underlying API.
         * @return Opaque pointer token for the GL program.
         * @since 1.0
         */
        [[nodiscard]] void* GetNativeHandle() const override;

    private:
        unsigned int m_ProgramID = 0;  // GL program name (GLuint)
        std::string  m_Name;
        bool         m_Valid = false;

        /**
         * @brief Load, compile, and link shader sources into a program.
         * @param vertexPath   Vertex shader source path.
         * @param fragmentPath Fragment shader source path.
         * @return true on success.
         * @pre Valid OpenGL context current.
         */
        bool LoadAndCompile(const std::string& vertexPath,
            const std::string& fragmentPath);

        /**
         * @brief Compile a single shader stage.
         * @param type   GL stage enum (for example GL_VERTEX_SHADER).
         * @param source GLSL source text.
         * @return Compiled shader object name (GLuint), or 0 on failure.
         * @pre Valid OpenGL context current.
         */
        unsigned int CompileShader(unsigned int type, const std::string& source);

        /**
         * @brief Read a text file into memory.
         * @param path Filesystem path.
         * @return File contents as a string (empty on read failure).
         */
        std::string LoadFile(const std::string& path);
    };

} // namespace Interstellar::Renderers::OpenGL
