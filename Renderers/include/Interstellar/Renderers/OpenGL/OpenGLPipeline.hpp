#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Interstellar/Graphics/IRenderPipeline.hpp"

/**
 * @file
 * @ingroup Renderers
 * @brief OpenGL render pipeline implementation header.
 * @details
 *   Encapsulates shader and runtime-configurable pipeline options for the
 *   OpenGL backend. The native handle is opaque and intended only for
 *   backend interop.
 *
 *   Threading:
 *     Unless noted otherwise in the .cpp, calls are expected on the
 *     render/main thread while a valid GL context is current.
 */

 /**
  * @defgroup GraphicsOpenGL OpenGL Backend
  * @ingroup Renderers
  * @brief Types provided by the OpenGL renderer.
  * @since 1.0
  */

namespace Interstellar::Renderers::OpenGL {

    /**
     * @brief OpenGL-specific render pipeline object.
     * @ingroup GraphicsOpenGL
     * @details
     *   Binds a shader and exposes a flexible set of named options that
     *   describe fixed-function or semantic state (for example "depthTest",
     *   "cullMode", "alphaBlend", "pipelineLabel").
     *
     *   Notes:
     *   - GetNativeHandle() returns an opaque pointer. Treat it as a token.
     *   - Options are stored as PipelineOptionValue (bool, int, float, string).
     *
     * @since 1.0
     * @see Interstellar::Graphics::IRenderPipeline
     * @see Interstellar::Graphics::IShader
     */
    class OpenGLPipeline : public Interstellar::Graphics::IRenderPipeline {
    public:
        /**
         * @brief Construct a pipeline from a compiled shader.
         * @param shader Shader to associate with this pipeline (must be non-null and valid).
         * @since 1.0
         */
        explicit OpenGLPipeline(std::shared_ptr<Interstellar::Graphics::IShader> shader);

        /**
         * @brief Destroy the pipeline and any owned GL state.
         * @since 1.0
         */
        ~OpenGLPipeline();

        /**
         * @brief Access the shader bound to this pipeline.
         * @return Shared pointer to the associated shader.
         * @since 1.0
         */
        [[nodiscard]] std::shared_ptr<Interstellar::Graphics::IShader> GetShader() const override;

        /**
         * @brief Query a named pipeline option.
         * @param name Option key (for example "depthTest", "cullMode").
         * @return PipelineOptionValue (bool, int, float, string). If the option
         *         does not exist, the implementation may return a default value
         *         (for example, false or empty string).
         * @since 1.0
         */
        [[nodiscard]] Interstellar::Graphics::PipelineOptionValue
            GetOption(const std::string& name) const override;

        /**
         * @brief Backend-native handle (opaque).
         * @details For OpenGL this may represent a program ID or internal token.
         *          Do not dereference or cast outside backend code.
         * @return Opaque pointer to the native pipeline resource.
         * @since 1.0
         */
        [[nodiscard]] void* GetNativeHandle() const override;

        // Optional convenience: if you later add mutators, prefer typed helpers like:
        //   void SetDepthTest(bool enabled);
        //   void SetCullMode(int mode);
        //   void SetLabel(std::string name);
        // and keep the generic map as an implementation detail.

    private:
        std::shared_ptr<Interstellar::Graphics::IShader> m_Shader;
        std::unordered_map<std::string, Interstellar::Graphics::PipelineOptionValue> m_Options;
    };

} // namespace Interstellar::Renderers::OpenGL
