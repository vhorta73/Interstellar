#pragma once

#include <string>
#include <vector>

namespace Interstellar::Graphics::Core {

    class IShader {
    public:
        virtual ~IShader() = default;

        virtual const std::string& GetName() const = 0;

        // Instead of enum, use runtime query for supported stages
        virtual std::vector<std::string> GetAvailableStages() const = 0;

        virtual bool IsValid() const = 0;

        virtual void* GetNativeHandle() const = 0;
    };

}
