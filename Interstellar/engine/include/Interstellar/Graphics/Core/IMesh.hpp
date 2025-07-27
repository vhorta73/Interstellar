#pragma once

#include <cstddef>
#include <string_view>

namespace Interstellar::Graphics::Core {

    /// Represents a platform-agnostic mesh (vertex + index buffers)
    class IMesh {
    public:
        virtual ~IMesh() = default;

        /// Optional debug label for this mesh
        virtual std::string_view GetDebugName() const = 0;

        /// Number of vertices used by the mesh
        virtual size_t GetVertexCount() const = 0;

        /// Number of indices (usually for glDrawElements or equivalent)
        virtual size_t GetIndexCount() const = 0;

        /// Returns native handle (e.g., OpenGL VAO, Vulkan handle)
        virtual void* GetNativeHandle() const = 0;
    };

}
