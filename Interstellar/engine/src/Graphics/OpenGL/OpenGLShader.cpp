#include "OpenGLShader.hpp"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Interstellar/Core/Logging.hpp"

using namespace Interstellar::Graphics;
using namespace Interstellar::Graphics::OpenGL;

static const Interstellar::Core::Logger s_Logger(Interstellar::Core::LOG_GRAPHIC);

OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
    : m_Name(name) {
    m_Valid = LoadAndCompile(vertexPath, fragmentPath);
}

OpenGLShader::~OpenGLShader() {
    if (m_ProgramID != 0) {
        glDeleteProgram(m_ProgramID);
        m_ProgramID = 0;
    }
}

const std::string& OpenGLShader::GetName() const { return m_Name; }

std::vector<std::string> OpenGLShader::GetAvailableStages() const { return { "Vertex", "Fragment" }; }

bool OpenGLShader::IsValid() const { return m_Valid; }

void* OpenGLShader::GetNativeHandle() const {
    return reinterpret_cast<void*>(static_cast<uintptr_t>(m_ProgramID));
}

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
    return true;
}

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

        s_Logger.LogError("{} shader compilation failed for '{}' : {}", typeStr, m_Name, log);
        glDeleteShader(shader);
        return 0;
    }
    s_Logger.LogDebug("Shader '{}' compiled and linked successfully.", m_Name);
    return shader;
}

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
