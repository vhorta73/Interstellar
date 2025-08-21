#pragma once

#include <string>
#include <vector>

namespace Interstellar::Graphics {

    /**
     * @brief Interface for a platform-agnostic shader object.
     *
     * Represents a compiled shader or shader program, depending on the graphics backend.
     * This interface abstracts shader reflection, name identification, and low-level access.
     *
     * Implementations may correspond to a single-stage shader (e.g., vertex or fragment),
     * or a combined program object depending on the API (e.g., Vulkan pipelines, OpenGL programs).
     *
     * @since 1.0
     */
    class IShader {
    public:
        virtual ~IShader() = default;

        /**
         * @brief Retrieves the debug or symbolic name of the shader.
         *
         * Useful for diagnostics, logging, and UI-based introspection tools.
         *
         * @return A constant reference to the shader's name.
         *
         * @since 1.0
         */
        virtual const std::string& GetName() const = 0;

        /**
         * @brief Retrieves the list of shader stages available in this shader.
         *
         * Instead of relying on a fixed enum, this allows flexible runtime inspection.
         * For example: `{"vertex", "fragment"}` for GLSL, or `{"vs", "ps"}` for HLSL.
         *
         * @return A vector of stage names present in this shader.
         *
         * @since 1.0
         */
        virtual std::vector<std::string> GetAvailableStages() const = 0;

        /**
         * @brief Checks if the shader has been compiled and linked correctly.
         *
         * This is particularly useful in runtime-compilation environments,
         * or when loading shader binaries from disk.
         *
         * @return true if the shader is valid and usable.
         *
         * @since 1.0
         */
        virtual bool IsValid() const = 0;

        /**
         * @brief Returns the native graphics API handle for backend integration.
         *
         * The returned pointer type varies by API:
         * - OpenGL: GLuint (casted as void*) for shader program ID
         * - Vulkan: VkShaderModule or pipeline stage handle
         * - DirectX: ID3D12PipelineState or ID3D11PixelShader
         *
         * Consumers should treat the handle as opaque unless integrating directly
         * with the low-level API.
         *
         * @return A void pointer to the native shader resource.
         *
         * @since 1.0
         */
        virtual void* GetNativeHandle() const = 0;
    };
}
