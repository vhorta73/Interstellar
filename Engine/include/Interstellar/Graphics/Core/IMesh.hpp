#pragma once

#include <cstddef>
#include <string_view>

namespace Interstellar::Graphics::Core {

    /**
     * @brief Represents a platform-agnostic mesh containing vertex and index buffers.
     *
     * This interface abstracts the representation of a mesh within the rendering pipeline.
     * Implementations manage underlying GPU resources, such as vertex array objects (VAOs)
     * or buffer handles, and expose necessary metadata for rendering operations.
     * 
     * Typical use cases include static geometry, skinned models, or any renderable mesh data.
     *
     * @since 1.0
     */ 
    class IMesh {
    public:
        virtual ~IMesh() = default;

        /**
        * @brief Retrieves an optional debug name associated with the mesh.
        * 
        * Useful for profiling, logging, or graphics debugging purposes.
        * May return an empty string if no name is set.
        * 
        * @return A string view representing the debug level.
        * 
        * @since 1.0
        */
        virtual std::string_view GetDebugName() const = 0;

        /**
        * @brief Optionally sets a debug name for the mesh.
        *
        * Implementations may ignore this if not supported.
        *
        * @param name Debug label for the mesh.
        * @return True if the name was set successfully, and false otherwise or not supported.
        * @since 1.0
        */
        virtual bool SetDebugName(std::string /*name*/) { return false; } // not supported by default

        /**
        * @brief Gets the total number of vertices in the mesh.
        * 
        * This value is typically used in rendering calculations, buffer allocations,
        * or for computing bounding volumes.
        * 
        * @return The number of vertices.
        * 
        * @since 1.0
        */
        virtual size_t GetVertexCount() const = 0;

        /**
        * @brief Gets the total number of indices used by the mesh.
        * 
        * Relevant for indexed rendering operation (e.g., using index buffers).
        * If the mesh is rendered non-indexed, this may return zero.
        * 
        * @return The number of indices.
        * 
        * @since 1.0
        * @see GetVertexCount()
        */
        virtual size_t GetIndexCount() const = 0;

        /**
        * @brief Returns the native GPU handle for low-level access.
        * 
        * The actual handle type depends on the graphics backend:
        * - OpenGL: Vertex Array Object (VAO) or buffer handle.
        * - Vulkan: Buffer handle, descriptor, or mesh resource ID.
        * - DirectX: Vertex buffer pointer or resource interface.
        * 
        * This is intended for backend-specific usage and should be treated as opaque by consumers.
        * 
        * @return A void pointer to the native mesh handle.
        * 
        * @since 1.0
        */
        virtual void* GetNativeHandle() const = 0;
    };

}
