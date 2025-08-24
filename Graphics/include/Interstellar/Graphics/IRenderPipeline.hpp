#pragma once

#include <memory>
#include <string>
#include <variant>
#include "Interstellar/Graphics/IMaterial.hpp"

namespace Interstellar::Graphics {

    class IShader;
    class IMaterial;

    /**
     * @file
     * @ingroup Graphics
     * @brief Render pipeline interface and dynamic option facilities.
     * @details
     *   This header defines a backend-agnostic render pipeline abstraction and a
     *   small variant value type for introspecting pipeline options at runtime.
     *   Concrete backends (OpenGL, Vulkan, DirectX12, Metal) implement this API.
     */

     /**
      * @brief Variant type for dynamic pipeline options.
      * @ingroup Graphics
      * @details
      *   Enables flexible configuration and inspection of pipeline state across backends.
      *   Typical usages include feature toggles and mode selection.
      *
      *   Supported alternative types:
      *   - bool          (for toggles such as depthTest, alphaBlend)
      *   - int           (for enum-like modes or bitmasks such as cullMode)
      *   - float         (for thresholds or scalar parameters)
      *   - std::string   (for labels or identifiers)
      *
      *   Example option names (conventions only; backends may differ):
      *   - "depthTest"     -> bool
      *   - "alphaBlend"    -> bool
      *   - "cullMode"      -> int
      *   - "pipelineLabel" -> std::string
      *
      * @since 1.0
      */
    using PipelineOptionValue = std::variant<bool, int, float, std::string>;

    /**
     * @brief Platform-agnostic render pipeline interface.
     * @ingroup Graphics
     * @details
     *   Encapsulates the combination of shader stages and fixed-function state required
     *   to draw meshes. Pipelines are typically created from a compiled/linked shader
     *   and may own descriptor layouts, vertex formats, blending, depth, and raster state.
     *
     * @thread_safety Unless otherwise stated by the implementation, all methods must be
     *                called on the render/main thread. Instances are not required to be
     *                thread-safe.
     * @since 1.0
     */
    class IRenderPipeline {
    public:
        virtual ~IRenderPipeline() = default;

        /**
         * @brief Get the shader associated with this pipeline.
         * @ingroup Graphics
         * @return Shared pointer to the bound IShader (may be null if not applicable).
         * @since 1.0
         * @see IShader
         */
        virtual std::shared_ptr<IShader> GetShader() const = 0;

        /**
         * @brief Query a dynamic pipeline option by name.
         * @ingroup Graphics
         * @param name Option key (case and supported keys are backend-defined).
         * @return PipelineOptionValue containing the current value.
         * @throws std::out_of_range Implementations may throw if the option name is unknown.
         * @throws std::invalid_argument Implementations may throw if the type is unsupported.
         * @note Keys and their types are implementation-specific; prefer feature detection
         *       via the renderer factory or backend docs when portability is critical.
         * @since 1.0
         * @see PipelineOptionValue
         */
        virtual PipelineOptionValue GetOption(const std::string& name) const = 0;

        /**
         * @brief Get a backend-native pipeline handle (opaque).
         * @ingroup Graphics
         * @details
         *   Intended for low-level integration or debug tooling. The concrete type depends
         *   on the backend, for example:
         *   - OpenGL: program or pipeline id (cast to void*)
         *   - Vulkan: VkPipeline
         *   - DirectX12: ID3D12PipelineState*
         * @return Opaque pointer to the native pipeline object (may be null).
         * @since 1.0
         */
        virtual void* GetNativeHandle() const = 0;

        // NEW: non-pure, default implementation returns a generic fallback.
        // Backends can override later to return a native material.
        virtual std::shared_ptr<IMaterial> CreateMaterial() {
            return std::make_shared<FallbackMaterial>();
        }
    };

} // namespace Interstellar::Graphics
