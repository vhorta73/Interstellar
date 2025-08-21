#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Interstellar::Renderers::Vulkan
{

    struct ExtensionList
    {
        std::vector<std::string> layers;
        std::vector<std::string> extensions;
    };

    struct ContextCreateInfo
    {
        std::string appName;
        std::vector<std::string> requiredExtensions;
        std::vector<std::string> requiredLayers;
        bool enableValidation = false;
        std::shared_ptr<class IExtensionSelector> extensionSelector;
        // future hooks: allocators, debug callbacks, etc.
    };

    class IContext
    {
    public:
        virtual ~IContext() = default;

        /// Returns the raw VkInstance handle
        /// (as an opaque pointer so no Vk headers leak into user code)
        virtual void* GetNativeHandle() const = 0;

        /// Waits for the instance (all devices) to go idle
        virtual void  WaitIdle() const = 0;

        // TODO: CreateDevice(), CreateSurface(), etc.
    };

    class InstanceBuilder
    {
    public:
        /// Creates and returns a new Vulkan context, or throws on failure
        static std::unique_ptr<IContext> Create(ContextCreateInfo const& ci);
    };

} // namespace Interstellar::Graphics::Vulkan
