#pragma once

#include <string>
#include <vector>

namespace Interstellar::Graphics {

    /**
     * @file
     * @ingroup Graphics
     * @brief Shader interface for backend-agnostic rendering.
     * @details
     *   Represents a compiled shader or a linked program, depending on the
     *   graphics API. Concrete backends (OpenGL, Vulkan, DirectX12, Metal)
     *   implement this interface to expose shader identity, stage listing,
     *   validation status, and an optional native handle for low-level use.
     */

     /**
      * @brief Platform-agnostic shader interface.
      * @ingroup Graphics
      * @details
      *   Implementations may wrap a single stage (for example, vertex or fragment)
      *   or a multi-stage program object. Unless stated otherwise, all methods
      *   are expected to be called from the render/main thread.
      * @since 1.0
      */
    class IShader {
    public:
        virtual ~IShader() = default;

        /**
         * @brief Get the debug or symbolic name of the shader.
         * @ingroup Graphics
         * @details Useful for diagnostics, logging, and editor/inspector UIs.
         *          May be empty if the backend does not track names.
         * @return Constant reference to the shader name string.
         * @since 1.0
         */
        virtual const std::string& GetName() const = 0;

        /**
         * @brief List the shader stages present in this object.
         * @ingroup Graphics
         * @details Returns backend-specific stage identifiers. Examples:
         *   - GLSL: {"vertex", "fragment"}
         *   - HLSL: {"vs", "ps"}
         *   - Metal: {"vertex", "fragment"}
         *   Implementations should return stable, lowercase tokens when possible.
         * @return Vector of stage name strings.
         * @since 1.0
         */
        virtual std::vector<std::string> GetAvailableStages() const = 0;

        /**
         * @brief Query whether the shader is compiled/linked and usable.
         * @ingroup Graphics
         * @details For runtime compilation or binary loading workflows, this
         *          indicates that all required stages loaded and any linking
         *          or pipeline creation succeeded.
         * @return true if valid and ready for use; false otherwise.
         * @since 1.0
         */
        virtual bool IsValid() const = 0;

        /**
         * @brief Get a backend-native shader/program handle (opaque).
         * @ingroup Graphics
         * @details Intended for advanced integration or debugging. The actual
         *          type depends on the backend and should be treated as opaque:
         *   - OpenGL: program id cast to void*
         *   - Vulkan: VkShaderModule or stage/pipeline object
         *   - DirectX: ID3D12PipelineState* or ID3D11*Shader
         *          May return nullptr if not applicable.
         * @return Opaque native handle pointer (or nullptr).
         * @since 1.0
         */
        virtual void* GetNativeHandle() const = 0;
    };

} // namespace Interstellar::Graphics
