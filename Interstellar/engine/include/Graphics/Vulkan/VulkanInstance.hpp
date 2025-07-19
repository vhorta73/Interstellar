#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <cstring>

namespace Graphics::Vulkan {
    class VulkanInstance {

    public:
         /*
         * Constructs a VulkanInstance.
         * @param appName Name of the application (used for VkApplicationInfo).
         * @param extensions List of Vulkan extensions to enable (e.g., surface, debug utils).
         * @param layers List of validation layers to enable (e.g., VK_LAYER_KHRONOS_validation).
         */
        VulkanInstance(
            const char* appName,
            const std::vector<const char*>& extensions = {},
            const std::vector<const char*>& layers = {}
        );

        // Non-copyable, non-movable
        VulkanInstance(const VulkanInstance&) = delete;
        VulkanInstance& operator=(const VulkanInstance&) = delete;

        /**
         * Cleans up the Vulkan instance.
         */
        ~VulkanInstance();

        /**
         * Retrieves the underlying VkInstance handle.
         */
        VkInstance get() const { return instance_; }

    private:
        void createInstance(
            const char* appName,
            const std::vector<const char*>& extensions,
            const std::vector<const char*>& layers
        );

        VkInstance instance_ = VK_NULL_HANDLE;
    };
}