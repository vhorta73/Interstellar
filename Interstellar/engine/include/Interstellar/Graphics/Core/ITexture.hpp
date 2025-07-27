#pragma once

#include <cstdint>
#include <string>

namespace Interstellar::Graphics::Core {

    class ITexture {
    public:
        virtual ~ITexture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        // Use string for format to allow for custom formats like "RGBA8", "BC7", etc.
        virtual std::string GetFormat() const = 0;

        virtual const std::string& GetName() const = 0;

        virtual void* GetNativeHandle() const = 0;
    };

}
