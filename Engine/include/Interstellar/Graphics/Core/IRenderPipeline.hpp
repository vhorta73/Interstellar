#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace Interstellar::Graphics::Core {

    class IShader;

    /**
     * @brief Variant type used for dynamic pipeline options.
     *
     * This allows flexible configuration and introspection of pipeline parameters
     * across different graphics APIs or runtime settings.
     *
     * Supported types include:
     * - `bool` - for toggles (e.g., depth testing)
     * - `int`  - for enums or masks
     * - `float` - for scalar thresholds or scales
     * - `std::string` - for names or identifiers
     *
     * @since 1.0
     */
    using PipelineOptionValue = std::variant<bool, int, float, std::string>;

    /**
     * @brief Interface for a platform-agnostic render pipeline object.
     *
     * Encapsulates the combination of shader stages and fixed-function state
     * necessary to render mesh geometry. This abstraction allows backends to
     * configure and bind graphics state in a unified way.
     *
     * @since 1.0
     */
    class IRenderPipeline {
    public:
        virtual ~IRenderPipeline() = default;

        /**
         * @brief Retrieves the shader associated with this pipeline.
         *
         * This is the shader used to configure pipeline state or execute draw calls.
         *
         * @return A shared pointer to the bound @c IShader.
         *
         * @since 1.0
         * @see IShader
         */
        virtual std::shared_ptr<IShader> GetShader() const = 0;

        /**
         * @brief Queries a dynamic pipeline option by name.
         *
         * Allows for flexible retrieval of pipeline configuration parameters at runtime.
         * This avoids rigid enum/struct systems and promotes introspectable state.
         *
         * Common option names might include:
         * - `"depthTest"` -> `bool`
         * - `"cullMode"` -> `int`
         * - `"alphaBlend"` -> `bool`
         * - `"pipelineLabel"` -> `std::string`
         *
         * @param name The name of the pipeline option.
         * @return The value of the pipeline option as a @c PipelineOptionValue.
         *
         * @since 1.0
         * @see PipelineOptionValue
         */
        virtual PipelineOptionValue GetOption(const std::string& name) const = 0;

        /**
         * @brief Returns the native handle to the platform-specific pipeline object.
         *
         * The handle type depends on the graphics backend:
         * - OpenGL: GLuint program ID
         * - Vulkan: VkPipeline
         * - DirectX: ID3D12PipelineState
         *
         * This should be treated as an opaque pointer unless used in backend code.
         *
         * @return A void pointer to the native pipeline resource.
         *
         * @since 1.0
         */
        virtual void* GetNativeHandle() const = 0;
    };
}
