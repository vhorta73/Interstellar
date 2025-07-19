#pragma once

#include <vector>
#include <algorithm>
#include <vulkan/vulkan.h>

namespace Graphics::Vulkan {

    // DefaultPreference traits: specialize per Item
    template<typename Item>
    struct DefaultPreference;

    // Default for VkLayerProperties: accept all, prefer highest implementationVersion
    template<>
    struct DefaultPreference<VkLayerProperties> {
        bool IsAcceptable(const VkLayerProperties&) const {
            return true;
        }
        bool operator()(const VkLayerProperties& a, const VkLayerProperties& b) const {
            return a.implementationVersion > b.implementationVersion;
        }
    };

    // Default for VkExtensionProperties: accept all, prefer highest specVersion
    template<>
    struct DefaultPreference<VkExtensionProperties> {
        bool IsAcceptable(const VkExtensionProperties&) const {
            return true;
        }
        bool operator()(const VkExtensionProperties& a, const VkExtensionProperties& b) const {
            return a.specVersion > b.specVersion;
        }
    };

    /**
     * @brief A generic Vulkan selector for lists of items.
     *
     * @tparam Item       The type of items to select (VkLayerProperties or VkExtensionProperties).
     * @tparam Preference A functor defining selection preferences;
     *                    defaults to DefaultPreference<Item>.
     */
    template<typename Item, typename Preference = DefaultPreference<Item>>
    class VulkanSelector {
    public:
        using Items = std::vector<Item>;

        // Core select: filter then sort
        static Items Select(const Preference& pref, const Items& available) {
            Items filtered;
            filtered.reserve(available.size());
            for (const auto& item : available) {
                if (pref.IsAcceptable(item)) {
                    filtered.push_back(item);
                }
            }
            std::sort(filtered.begin(), filtered.end(),
                [&](const Item& a, const Item& b) { return pref(a, b); });
            return filtered;
        }

        // Overload using default preference
        static Items Select(const Items& available) {
            return Select(Preference{}, available);
        }
    };

    // Aliases for layers and extensions
    using LayerSelector = VulkanSelector<VkLayerProperties>;
    using ExtensionSelector = VulkanSelector<VkExtensionProperties>;

    // Helpers to enumerate and select lists

    template<typename Pref = DefaultPreference<VkLayerProperties>>
    inline std::vector<VkLayerProperties> SelectAvailableLayers(const Pref& pref = Pref{}) {
        uint32_t count = 0;
        vkEnumerateInstanceLayerProperties(&count, nullptr);
        std::vector<VkLayerProperties> props(count);
        vkEnumerateInstanceLayerProperties(&count, props.data());
        return LayerSelector::Select(pref, props);
    }

    template<typename Pref = DefaultPreference<VkExtensionProperties>>
    inline std::vector<VkExtensionProperties> SelectAvailableExtensions(
        const char* layerName = nullptr,
        const Pref& pref = Pref{})
    {
        uint32_t count = 0;
        vkEnumerateInstanceExtensionProperties(layerName, &count, nullptr);
        std::vector<VkExtensionProperties> props(count);
        vkEnumerateInstanceExtensionProperties(layerName, &count, props.data());
        return ExtensionSelector::Select(pref, props);
    }

    // Single-item selectors: pick the first (highest-preference) or throw if none

    /**
     * @brief Select the top-preferred instance layer.
     * @throws std::runtime_error if no layers are available.
     */
    template<typename Pref = DefaultPreference<VkLayerProperties>>
    inline VkLayerProperties SelectLayer(const Pref& pref = Pref{}) {
        auto list = SelectAvailableLayers(pref);
        if (list.empty()) {
            throw std::runtime_error("No Vulkan instance layers available");
        }
        return list.front();
    }

    /**
     * @brief Select the top-preferred instance extension, optionally for a layer.
     * @throws std::runtime_error if no extensions are available.
     */
    template<typename Pref = DefaultPreference<VkExtensionProperties>>
    inline VkExtensionProperties SelectExtension(
        const char* layerName = nullptr,
        const Pref& pref = Pref{})
    {
        auto list = SelectAvailableExtensions(layerName, pref);
        if (list.empty()) {
            throw std::runtime_error("No Vulkan instance extensions available");
        }
        return list.front();
    }
}