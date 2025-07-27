#pragma once

#include "Interstellar/Graphics/Core/IShader.hpp"
#include <string>
#include <vector>

namespace Interstellar::Graphics::OpenGL {

    class OpenGLShader : public Core::IShader {
    public:
        OpenGLShader(const std::string& vertexPath, const std::string& fragmentPath);
        ~OpenGLShader();

        const std::string& GetName() const override;
        std::vector<std::string> GetAvailableStages() const override;
        bool IsValid() const override;
        void* GetNativeHandle() const override;

    private:
        unsigned int m_ProgramID = 0;
        std::string m_Name;
        bool m_Valid = false;

        bool CompileShader(const std::string& vertexSrc, const std::string& fragmentSrc);
        std::string LoadFile(const std::string& path);
    };

}
