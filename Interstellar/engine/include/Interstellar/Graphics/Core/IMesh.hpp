#pragma once

#include <cstddef>

namespace Interstellar::Graphics::Core {

    class IMesh {
    public:
        virtual ~IMesh() = default;

        virtual const char* GetDebugName() const = 0;

        virtual size_t GetVertexCount() const = 0;
        virtual size_t GetIndexCount() const = 0;

        virtual void* GetNativeHandle() const = 0;
    };

}
