#include "OpenGLPipeline.hpp"

#include "Interstellar/Core/Logging.hpp"

using namespace Interstellar::Graphics::OpenGL;
static const Interstellar::Core::Logger s_Logger(Interstellar::Core::LOG_GRAPHIC);

OpenGLPipeline::OpenGLPipeline(std::shared_ptr<Core::IShader> shader)
    : m_Shader(std::move(shader)) {
    m_Options["DepthTest"] = true;
    m_Options["BlendEnabled"] = false;
}

OpenGLPipeline::~OpenGLPipeline() = default;

std::shared_ptr<Interstellar::Graphics::Core::IShader> OpenGLPipeline::GetShader() const {
    return m_Shader;
}

Interstellar::Graphics::Core::PipelineOptionValue OpenGLPipeline::GetOption(const std::string& name) const {
    auto it = m_Options.find(name);
    if (it != m_Options.end()) return it->second;
    s_Logger.LogWarn("Pipeline option '{}' not found.", name);
    return {}; // Option not found
}

void* OpenGLPipeline::GetNativeHandle() const {
    return nullptr; // Not needed yet
}
