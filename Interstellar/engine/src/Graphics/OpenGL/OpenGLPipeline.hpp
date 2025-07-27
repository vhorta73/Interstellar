#pragma once

#include "Interstellar/Graphics/Core/IRenderPipeline.hpp"
#include <memory>
#include <unordered_map>
#include <variant>

namespace Interstellar::Graphics::OpenGL {

    class OpenGLShader;

    class OpenGLPipeline : public Core::IRenderPipeline {
    public:
        OpenGLPipeline(std::shared_ptr<Core::IShader> shader);
        ~OpenGLPipeline();

        std::shared_ptr<Core::IShader> GetShader() const override;
        Core::PipelineOptionValue GetOption(const std::string& name) const override;
        void* GetNativeHandle() const override;

    private:
        std::shared_ptr<Core::IShader> m_Shader;
        std::unordered_map<std::string, Core::PipelineOptionValue> m_Options;
    };

}
