#include "OpenGLPipeline.hpp"
#include "Interstellar/Core/Logging.hpp"

using namespace Interstellar::Graphics::OpenGL;

// Static logger for pipeline-related diagnostics.
static const Interstellar::Core::Logger s_Logger(Interstellar::Core::LOG_GRAPHIC);

namespace Interstellar::Graphics::OpenGL {

    /**
     * @brief Constructs an OpenGLPipeline with a provided shader and default options.
     *
     * Default options include:
     * - DepthTest = true
     * - BlendEnabled = false
     *
     * These can be queried dynamically using GetOption().
     *
     * @param shader The shader program to associate with this pipeline.
     * @since 1.0
     */
    OpenGLPipeline::OpenGLPipeline(std::shared_ptr<Core::IShader> shader)
        : m_Shader(std::move(shader))
    {
        m_Options["DepthTest"] = true;
        m_Options["BlendEnabled"] = false;
    }

    /**
     * @brief Destructor for the OpenGL pipeline.
     * @since 1.0
     */
    OpenGLPipeline::~OpenGLPipeline() = default;

    /**
     * @brief Returns the shader bound to this pipeline.
     * @return Shared pointer to the IShader used in this pipeline.
     * @since 1.0
     */
    std::shared_ptr<Core::IShader> OpenGLPipeline::GetShader() const {
        return m_Shader;
    }

    /**
     * @brief Retrieves a pipeline configuration option by name.
     *
     * Typical options may include:
     * - "DepthTest": bool
     * - "BlendEnabled": bool
     *
     * @param name The name of the pipeline option.
     * @return The associated PipelineOptionValue, or a default-initialized variant if not found.
     * @since 1.0
     */
    Core::PipelineOptionValue OpenGLPipeline::GetOption(const std::string& name) const {
        auto it = m_Options.find(name);
        if (it != m_Options.end()) {
            return it->second;
        }

        s_Logger.LogWarn("Pipeline option '{}' not found.", name);
        return {}; // Returns default std::variant (monostate-like).
    }

    /**
     * @brief Returns a backend-specific handle for the pipeline.
     *
     * Currently returns nullptr since OpenGL doesn't use pipeline objects like Vulkan or DX12.
     *
     * @return Always nullptr.
     * @since 1.0
     */
    void* OpenGLPipeline::GetNativeHandle() const {
        return nullptr;
    }

} // namespace Interstellar::Graphics::OpenGL
