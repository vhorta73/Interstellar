#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace Interstellar::Graphics::Core {

    class IShader;

    using PipelineOptionValue = std::variant<bool, int, float, std::string>;

    class IRenderPipeline {
    public:
        virtual ~IRenderPipeline() = default;

        virtual std::shared_ptr<IShader> GetShader() const = 0;

        // Dynamic config instead of fixed struct
        virtual PipelineOptionValue GetOption(const std::string& name) const = 0;

        virtual void* GetNativeHandle() const = 0;
    };

}
