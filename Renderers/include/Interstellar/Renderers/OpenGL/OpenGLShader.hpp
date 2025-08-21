#pragma once

#include "Interstellar/Graphics/IShader.hpp"
#include <string>
#include <vector>

namespace Interstellar::Renderers::OpenGL {

    /**
     * @class OpenGLShader
     * @brief Represents an OpenGL shader program composed of vertex and fragment stages.
     *
     * This class implements the IShader interface using OpenGL's shader compilation and program linking.
     * It supports reflection for active stages and debug naming, and provides access to the native OpenGL handle.
     *
     * @since 1.0
     */
    class OpenGLShader : public Interstellar::Graphics::IShader {
    public:
        /**
         * @brief Constructs an OpenGL shader with source paths for vertex and fragment stages.
         *
         * @param name A debug/display name.
         * @param vertexPath Path to the vertex shader source file.
         * @param fragmentPath Path to the fragment shader source file.
         *
         * @since 1.0
         */
        OpenGLShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);

        /**
         * @brief Destructor. Releases OpenGL shader resources.
         *
         * @since 1.0
         */
        ~OpenGLShader();

        /**
         * @brief Returns the debug or display name of the shader.
         *
         * @return Reference to the internal shader name.
         * @since 1.0
         */
        [[nodiscard]] const std::string& GetName() const override;

        /**
         * @brief Returns the available shader stages in this program.
         *
         * Typically includes "vertex" and "fragment" stages.
         *
         * @return A vector of stage names present in this shader.
         * @since 1.0
         */
        [[nodiscard]] std::vector<std::string> GetAvailableStages() const override;

        /**
         * @brief Indicates whether the shader program compiled and linked successfully.
         *
         * @return true if the shader is valid and ready to use.
         * @since 1.0
         */
        [[nodiscard]] bool IsValid() const override;

        /**
         * @brief Returns the native OpenGL shader program handle.
         *
         * This is returned as a void pointer but can be cast to GLuint* by advanced users.
         *
         * @return Pointer to the OpenGL program ID.
         * @since 1.0
         */
        [[nodiscard]] void* GetNativeHandle() const override;

    private:
        unsigned int m_ProgramID = 0;
        std::string m_Name;
        bool m_Valid = false;

        /**
         * @brief Loads shader sources from file, compiles them, and links into a program.
         *
         * @param vertexPath Path to vertex shader.
         * @param fragmentPath Path to fragment shader.
         * @return true if compilation and linking succeeded.
         */
        bool LoadAndCompile(const std::string& vertexPath, const std::string& fragmentPath);

        /**
         * @brief Compiles a single OpenGL shader stage from source.
         *
         * @param type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
         * @param source GLSL source code as a string.
         * @return Compiled shader ID (GLuint).
         */
        unsigned int CompileShader(unsigned int type, const std::string& source);

        /**
         * @brief Loads a shader source file into a string.
         *
         * @param path Filesystem path to the shader source.
         * @return Shader source as a string.
         */
        std::string LoadFile(const std::string& path);
    };

}
