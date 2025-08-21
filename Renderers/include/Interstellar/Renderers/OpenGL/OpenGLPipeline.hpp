#pragma once

#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include <memory>
#include <unordered_map>
#include <string>

namespace Interstellar::Renderers::OpenGL {

    /**
     * @class OpenGLPipeline
     * @brief Represents an OpenGL-specific implementation of a render pipeline.
     *
     * This class encapsulates pipeline configuration and links to the associated shader program.
     * The configuration is abstracted as a set of named runtime options,
     * allowing flexibility without hardcoded states.
     *
     * Used in conjunction with mesh submission and rendering within the OpenGL backend.
     *
     * @since 1.0
     */
    class OpenGLPipeline : public Interstellar::Graphics::IRenderPipeline {
    public:
        /**
         * @brief Constructs a pipeline with a given shader.
         *
         * Stores a reference to the shader, which must be valid and compiled.
         *
         * @param shader The shader to associate with this pipeline.
         * @since 1.0
         */
        explicit OpenGLPipeline(std::shared_ptr<Interstellar::Graphics::IShader> shader);

        /**
         * @brief Destroys the pipeline and any associated OpenGL state.
         * @since 1.0
         */
        ~OpenGLPipeline();

        /**
         * @brief Returns the shader associated with this pipeline.
         *
         * Useful for pipeline introspection or shader-specific operations.
         *
         * @return A shared pointer to the associated shader.
         * @since 1.0
         * @see Core::IShader
         */
        [[nodiscard]] std::shared_ptr<Interstellar::Graphics::IShader> GetShader() const override;

        /**
         * @brief Retrieves a named pipeline option.
         *
         * These may represent user-defined rendering states or settings.
         * If the name is not found, the return may default-construct the value.
         *
         * @param name Name of the pipeline option.
         * @return The value associated with the given option name.
         * @since 1.0
         */
        [[nodiscard]] Interstellar::Graphics::PipelineOptionValue GetOption(const std::string& name) const override;

        /**
         * @brief Returns the native OpenGL handle used internally.
         *
         * May be cast to a GLuint or similar backend-specific type.
         * Treated as opaque outside of the graphics module.
         *
         * @return A void pointer to the native OpenGL pipeline handle.
         * @since 1.0
         */
        [[nodiscard]] void* GetNativeHandle() const override;

    private:
        std::shared_ptr<Interstellar::Graphics::IShader> m_Shader;
        std::unordered_map<std::string, Interstellar::Graphics::PipelineOptionValue> m_Options;
    };

}
