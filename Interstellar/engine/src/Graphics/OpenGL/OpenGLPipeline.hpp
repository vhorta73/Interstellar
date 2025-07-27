#pragma once

#include "Interstellar/Graphics/Core/IRenderPipeline.hpp"
#include <memory>
#include <unordered_map>
#include <string>

namespace Interstellar::Graphics::OpenGL {

    /// Represents an OpenGL-specific implementation of a render pipeline.
    class OpenGLPipeline : public Core::IRenderPipeline {
    public:
        /// Constructs a pipeline with a given shader.
        explicit OpenGLPipeline(std::shared_ptr<Core::IShader> shader);

        /// Destroys the pipeline.
        ~OpenGLPipeline();

        /// Returns the shader associated with this pipeline.
        std::shared_ptr<Core::IShader> GetShader() const override;

        /// Retrieves a pipeline option by name.
        Core::PipelineOptionValue GetOption(const std::string& name) const override;

        /// Returns the native OpenGL handle.
        void* GetNativeHandle() const override;

    private:
        std::shared_ptr<Core::IShader> m_Shader;
        std::unordered_map<std::string, Core::PipelineOptionValue> m_Options;
    };
}
