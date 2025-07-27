#pragma once

#include "Interstellar/Graphics/Core/IShader.hpp"
#include <string>
#include <vector>

namespace Interstellar::Graphics::OpenGL {

    /// Represents an OpenGL shader program composed of vertex and fragment shaders.
    class OpenGLShader : public Core::IShader {
    public:
        /// Constructs a shader with provided source file paths.
        /// @param name A debug/display name.
        /// @param vertexPath Path to the vertex shader source.
        /// @param fragmentPath Path to the fragment shader source.
        OpenGLShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);

        /// Releases OpenGL shader resources.
        ~OpenGLShader();

        /// Returns the user-defined name of the shader.
        [[nodiscard]] const std::string& GetName() const override;

        /// Returns a list of available shader stages (e.g., "vertex", "fragment").
        [[nodiscard]] std::vector<std::string> GetAvailableStages() const override;

        /// Checks if the shader compiled and linked successfully.
        [[nodiscard]] bool IsValid() const override;

        /// Returns the native OpenGL program ID.
        [[nodiscard]] void* GetNativeHandle() const override;

    private:
        unsigned int m_ProgramID = 0;
        std::string m_Name;
        bool m_Valid = false;

        /// Loads and compiles shaders from files and links the program.
        bool LoadAndCompile(const std::string& vertexPath, const std::string& fragmentPath);

        /// Compiles an individual shader from source.
        unsigned int CompileShader(unsigned int type, const std::string& source);

        /// Loads shader source from file.
        std::string LoadFile(const std::string& path);
    };
}
