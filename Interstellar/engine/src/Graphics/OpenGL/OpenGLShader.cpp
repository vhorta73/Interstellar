#include "OpenGLShader.hpp"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Interstellar/Core/Logging.hpp"

using namespace Interstellar::Graphics;
using namespace Interstellar::Graphics::OpenGL;

// Static logger instance used for shader-related logging.
static const Interstellar::Core::Logger s_Logger(Interstellar::Core::LOG_GRAPHIC);

/**
 * @brief Constructs an OpenGL shader program by loading and compiling vertex/fragment stages.
 * @param name A user-defined debug name.
 * @param vertexPath Path to the vertex shader source file.
 * @param fragmentPath Path to the fragment shader source file.
 * @since 1.0
 */
OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
    : m_Name(name) {
    m_Valid = LoadAndCompile(vertexPath, fragmentPath);
}

/**
 * @brief Destructor that releases the OpenGL program resource.
 * @since 1.0
 */
OpenGLShader::~OpenGLShader() {
    if (m_ProgramID != 0) {
        glDeleteProgram(m_ProgramID);
        m_ProgramID = 0;
    }
}

/**
 * @brief Gets the debug/display name of the shader.
 * @return A const reference to the shader name string.
 * @since 1.0
 */
const std::string& OpenGLShader::GetName() const { return m_Name; }

/**
 * @brief Returns the available shader stages for this program.
 * @return A vector of stage names: { "Vertex", "Fragment" }.
 * @since 1.0
 */
std::vector<std::string> OpenGLShader::GetAvailableStages() const { return { "Vertex", "Fragment" }; }

/**
 * @brief Indicates if the shader compiled and linked without errors.
 * @return True if the shader program is valid.
 * @since 1.0
 */
bool OpenGLShader::IsValid() const { return m_Valid; }

/**
 * @brief Returns the OpenGL program ID as an opaque handle.
 * @return A void pointer representing the OpenGL program.
 * @since 1.0
 */
void* OpenGLShader::GetNativeHandle() const {
    return reinterpret_cast<void*>(static_cast<uintptr_t>(m_ProgramID));
}

/**
 * @brief Loads and compiles the shader sources and links the program.
 * @param vertexPath Path to the vertex shader source.
 * @param fragmentPath Path to the fragment shader source.
 * @return True if successful; false on error.
 * @since 1.0
 */
bool OpenGLShader::LoadAndCompile(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = LoadFile(vertexPath);
    std::string fragmentCode = LoadFile(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty()) {
        s_Logger.LogError("Shader source is empty (vertex: '{}', fragment: '{}')", vertexPath, fragmentPath);
        return false;
    }

    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);

    if (!vertexShader || !fragmentShader) return false;

    m_ProgramID = glCreateProgram();
    glAttachShader(m_ProgramID, vertexShader);
    glAttachShader(m_ProgramID, fragmentShader);
    glLinkProgram(m_ProgramID);

    int success = 0;
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);

    if (!success) {
        char log[1024];
        glGetProgramInfoLog(m_ProgramID, sizeof(log), nullptr, log);
        s_Logger.LogError("Shader program link failed for '{}': {}", m_Name, log);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_ProgramID);
        m_ProgramID = 0;
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    s_Logger.LogDebug("Shader '{}' compiled and linked successfully.", m_Name);
    return true;
}

/**
 * @brief Compiles an individual shader stage.
 * @param type The OpenGL shader type (e.g., GL_VERTEX_SHADER).
 * @param source GLSL source code.
 * @return Compiled shader ID, or 0 if compilation failed.
 * @since 1.0
 */
unsigned int OpenGLShader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);

        const char* typeStr = (type == GL_VERTEX_SHADER)
            ? "Vertex"
            : (type == GL_FRAGMENT_SHADER)
            ? "Fragment"
            : "Unknown";

        s_Logger.LogError("{} shader compilation failed for '{}': {}", typeStr, m_Name, log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

/**
 * @brief Reads a text file from disk into a string.
 * @param path Path to the file.
 * @return File contents as a string, or empty string on error.
 * @since 1.0
 */
std::string OpenGLShader::LoadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        s_Logger.LogError("Failed to open shader file: {}", path);
        return {};
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
