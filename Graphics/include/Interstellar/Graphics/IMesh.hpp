#pragma once

#include <cstddef>     // size_t
#include <string_view>

namespace Interstellar::Graphics {

    /**
     * @file
     * @ingroup Graphics
     * @brief Mesh interface and metadata for backend-agnostic rendering.
     * @details
     *   Abstracts a renderable mesh (vertex/index data) managed by a graphics backend.
     *   Implementations own the GPU resources and expose minimal metadata required by
     *   the renderer. Typical uses include static geometry and skinned/instanced models.
     */

     /**
      * @brief Platform-agnostic mesh interface (vertex and index buffers).
      * @ingroup Graphics
      * @details
      *   Backends (OpenGL, Vulkan, DirectX12, Metal) implement this interface to
      *   encapsulate GPU resource ownership. Unless stated otherwise, all methods
      *   must be called on the render/main thread.
      * @since 1.0
      */
    class IMesh {
    public:
        virtual ~IMesh() = default;

        /**
         * @brief Retrieve the optional debug name associated with this mesh.
         * @ingroup Graphics
         * @details Intended for profiling, logging, and GPU debuggers. May be empty
         *          if no name is set or if the backend does not support debug labels.
         * @return String view of the debug name; empty view if unset/unsupported.
         * @since 1.0
         */
        virtual std::string_view GetDebugName() const = 0;

        /**
         * @brief Set a debug name/label for this mesh (optional).
         * @ingroup Graphics
         * @details Backends may copy the label into driver objects (for example,
         *          object labels in GPU debuggers). Implementations may ignore the call.
         * @param name Debug label to associate with the mesh (non-owning view).
         * @return true if the name was applied; false if unsupported or rejected.
         * @since 1.0
         */
        virtual bool SetDebugName(std::string_view /*name*/) { return false; } // default: unsupported

        /**
         * @brief Get the total number of vertices in the mesh.
         * @ingroup Graphics
         * @return Vertex count.
         * @since 1.0
         */
        virtual size_t GetVertexCount() const = 0;

        /**
         * @brief Get the total number of indices used by the mesh.
         * @ingroup Graphics
         * @details For non-indexed meshes this may be zero. For indexed draws,
         *          this is typically the element count used by the index buffer.
         * @return Index count (0 for non-indexed meshes).
         * @see GetVertexCount()
         * @since 1.0
         */
        virtual size_t GetIndexCount() const = 0;

        /**
         * @brief Get a backend-specific native handle (opaque).
         * @ingroup Graphics
         * @details The handle type is backend dependent:
         *   - OpenGL: VAO or buffer object id (cast to void*).
         *   - Vulkan: buffer/descriptor/resource handle.
         *   - DirectX: resource interface pointer or handle.
         *   Consumers should treat this as opaque and avoid relying on its type.
         *   May return nullptr if not applicable.
         * @return Opaque native handle pointer (or nullptr).
         * @since 1.0
         */
        virtual void* GetNativeHandle() const = 0;
    };

} // namespace Interstellar::Graphics
