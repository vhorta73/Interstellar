#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <algorithm>

namespace Interstellar {
    namespace Graphics {
        namespace Vulkan {
            namespace Selectors {
                //
                // Your preferred hard-coded combos:
                // for each layer, a ranked list of extensions;
                // for each extension, a ranked list of layers.
                //
                static const auto& PreferredLayerExtensionCombos()
                {
                    static const std::vector<std::pair<const char*, std::vector<const char*>>> combos =
                    {
                        // 1) The official umbrella validation layer
                        { "VK_LAYER_KHRONOS_validation",
                            {
                                "VK_EXT_debug_utils",             // modern debug callbacks
                                "VK_EXT_validation_features",     // finer-grained validation control
                                "VK_EXT_debug_report",            // legacy debug callback
                                "VK_KHR_surface",                 // required for windowing
                                "VK_KHR_win32_surface"            // on Windows
                            }
                        },
                        // 2) API call dumper (print every vk* call)
                        { "VK_LAYER_LUNARG_api_dump",
                            {
                                "VK_KHR_surface",
                                "VK_KHR_win32_surface"
                            }
                        },
                        // 3) Threading layer (checks Vulkan threading rules)
                        { "VK_LAYER_LUNARG_threading",
                            {
                                "VK_KHR_surface",
                                "VK_KHR_win32_surface"
                            }
                        },
                        // 4) Device-simulation (injects small GPU quirks)
                        { "VK_LAYER_LUNARG_device_simulation",
                            {
                                "VK_KHR_surface",
                                "VK_KHR_win32_surface",
                                "VK_KHR_get_physical_device_properties2"
                            }
                        },
                        // 5) Screenshot capture
                        { "VK_LAYER_LUNARG_screenshot",
                            {
                                "VK_KHR_surface",
                                "VK_KHR_win32_surface"
                            }
                        },
                        // 6) RenderDoc capture layer
                        { "VK_LAYER_RENDERDOC_Capture",
                            {
                                "VK_KHR_surface",
                                "VK_KHR_win32_surface"
                            }
                        },
                        // 7) GFXReconstruct capture layer
                        { "VK_LAYER_LUNARG_gfxreconstruct",
                            {
                                "VK_KHR_surface",
                                "VK_KHR_win32_surface"
                            }
                        }
                    };
                    return combos;
                };

                static const auto& PreferredExtensionLayerCombos() {
                    static const std::vector<std::pair<const char*, std::vector<const char*>>> combos = {
                    { "VK_EXT_debug_utils",
                        {
                            "VK_LAYER_KHRONOS_validation",
                        }
                    },
                    { "VK_EXT_validation_features",
                        {
                            "VK_LAYER_KHRONOS_validation",
                        }
                    },
                    { "VK_EXT_debug_report",
                        {
                            "VK_LAYER_KHRONOS_validation"
                        }
                    },
                    { "VK_KHR_surface",
                        {
                            "VK_LAYER_KHRONOS_validation",
                            "VK_LAYER_LUNARG_api_dump",
                            "VK_LAYER_LUNARG_threading",
                            "VK_LAYER_LUNARG_device_simulation",
                            "VK_LAYER_LUNARG_screenshot",
                            "VK_LAYER_RENDERDOC_Capture",
                            "VK_LAYER_LUNARG_gfxreconstruct"
                        }
                    },
                    { "VK_KHR_win32_surface",
                        {
                            "VK_LAYER_KHRONOS_validation",
                            "VK_LAYER_LUNARG_api_dump",
                            "VK_LAYER_LUNARG_threading",
                            "VK_LAYER_LUNARG_device_simulation",
                            "VK_LAYER_LUNARG_screenshot",
                            "VK_LAYER_RENDERDOC_Capture",
                           "VK_LAYER_LUNARG_gfxreconstruct"
                        }
                    },
                    { "VK_KHR_get_physical_device_properties2",
                        {
                            "VK_LAYER_LUNARG_device_simulation",
                            "VK_LAYER_KHRONOS_validation"
                        }
                    }
                    };
                    return combos;
                };

                //
                // DefaultPreference traits: specialize per Item
                //
                template<typename Item>
                struct DefaultPreference;

                //
                // Default for VkLayerProperties: accept all, prefer highest implementationVersion
                //
                template<>
                struct DefaultPreference<VkLayerProperties> {
                    bool IsAcceptable(const VkLayerProperties&) const {
                        return true;
                    }
                    bool operator()(const VkLayerProperties& a, const VkLayerProperties& b) const {
                        return a.implementationVersion > b.implementationVersion;
                    }
                };

                //
                // Default for VkExtensionProperties: accept all, prefer highest specVersion
                //
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
                 * @brief A preference functor for selecting Vulkan items.
                 *
                 * Specialize this for custom selection logic.
                 *
                 * @tparam Item The type of item to select (VkLayerProperties or VkExtensionProperties).
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
                            [&](const Item& a, const Item& b) {
                                return pref(a, b);
                            });
                        return filtered;
                    }

                    // Overload using default preference
                    static Items Select(const Items& available) {
                        return Select(Preference{}, available);
                    }
                };

                //
                // Aliases for layers and extensions
                //
                using LayerSelector = VulkanSelector<VkLayerProperties>;
                using ExtensionSelector = VulkanSelector<VkExtensionProperties>;


                //
                // Helpers to enumerate and select lists
                //
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

                //
                // Fallback machinery: try each candidate in turn and record failures
                //
                namespace detail {

                    // SFINAE: does Pref::ReportResult(const char*, bool) exist?
                    template<typename T, typename = void>
                    struct has_report : std::false_type {};

                    template<typename T>
                    struct has_report<T, std::void_t<
                        decltype(std::declval<T>().ReportResult(std::declval<const char*>(), true))
                        >> : std::true_type {};

                } // namespace detail

                /**
                 * @brief Try each layer in preference order until vkCreateInstance succeeds.
                 *        Records failures via Pref::ReportResult(name, success) if available.
                 */
                template<typename Pref = DefaultPreference<VkLayerProperties>>
                inline VkLayerProperties SelectLayerWithFallback(const Pref& pref = Pref{}) {
                    auto list = SelectAvailableLayers(pref);
                    for (auto& lp : list) {
                        VkInstanceCreateInfo ci{};
                        ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                        static const char* layerNames[] = { lp.layerName };
                        ci.enabledLayerCount = 1;
                        ci.ppEnabledLayerNames = layerNames;
                        ci.enabledExtensionCount = 0;
                        ci.ppEnabledExtensionNames = nullptr;

                        VkInstance inst = VK_NULL_HANDLE;
                        VkResult  r = vkCreateInstance(&ci, nullptr, &inst);
                        bool      ok = (r == VK_SUCCESS);
                        if (ok) {
                            vkDestroyInstance(inst, nullptr);
                        }

                        if constexpr (detail::has_report<Pref>::value) {
                            const_cast<Pref&>(pref).ReportResult(lp.layerName, ok);
                        }

                        if (ok) {
                            return lp;
                        }
                    }
                    throw std::runtime_error("SelectLayerWithFallback: no usable layers found");
                }

                /**
                 * @brief Try each extension in preference order until vkCreateInstance succeeds.
                 *        Records failures via Pref::ReportResult(name, success) if available.
                 */
                template<typename Pref = DefaultPreference<VkExtensionProperties>>
                inline VkExtensionProperties SelectExtensionWithFallback(
                    const char* layerName = nullptr,
                    const Pref& pref = Pref{})
                {
                    auto list = SelectAvailableExtensions(layerName, pref);
                    for (auto& ep : list) {
                        VkInstanceCreateInfo ci{};
                        ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                        ci.enabledLayerCount = 0;
                        ci.ppEnabledLayerNames = nullptr;
                        static const char* extNames[] = { ep.extensionName };
                        ci.enabledExtensionCount = 1;
                        ci.ppEnabledExtensionNames = extNames;

                        VkInstance inst = VK_NULL_HANDLE;
                        VkResult  r = vkCreateInstance(&ci, nullptr, &inst);
                        bool      ok = (r == VK_SUCCESS);
                        if (ok) {
                            vkDestroyInstance(inst, nullptr);
                        }

                        if constexpr (detail::has_report<Pref>::value) {
                            const_cast<Pref&>(pref).ReportResult(ep.extensionName, ok);
                        }

                        if (ok) {
                            return ep;
                        }
                    }
                    throw std::runtime_error("SelectExtensionWithFallback: no usable extensions found");
                }

                //
                // New: Combined layer+extension selector using your preferred combo tables,
                // falling back to the single-item selectors if no mapping applies.
                //
                template<
                    typename PrefLayer = DefaultPreference<VkLayerProperties>,
                    typename PrefExt = DefaultPreference<VkExtensionProperties>
                >
                inline std::pair<VkLayerProperties, VkExtensionProperties>
                    SelectLayerAndExtensionCombo(const PrefLayer& layerPref = PrefLayer{},
                        const PrefExt& extPref = PrefExt{})
                {
                    auto layers = SelectAvailableLayers(layerPref);
                    auto exts = SelectAvailableExtensions(nullptr, extPref);

                    // 1) Layer-first mapping:
                    for (auto const& [lyName, extList] : PreferredLayerExtensionCombos()) {
                        auto lit = std::find_if(
                            layers.begin(), layers.end(),
                            [&](auto const& L) { return std::strcmp(L.layerName, lyName) == 0; }
                        );
                        if (lit != layers.end()) {
                            // pick extension from extList
                            for (auto const& en : extList) {
                                auto eit = std::find_if(
                                    exts.begin(), exts.end(),
                                    [&](auto const& E) { return std::strcmp(E.extensionName, en) == 0; }
                                );
                                if (eit != exts.end()) {
                                    return { *lit, *eit };
                                }
                            }
                            // fallback extension
                            return { *lit, SelectExtension(nullptr, extPref) };
                        }
                    }

                    // 2) Extension-first mapping:
                    for (auto const& [en, lyList] : PreferredExtensionLayerCombos()) {
                        auto eit = std::find_if(
                            exts.begin(), exts.end(),
                            [&](auto const& E) { return std::strcmp(E.extensionName, en) == 0; }
                        );
                        if (eit != exts.end()) {
                            for (auto const& ln : lyList) {
                                auto lit = std::find_if(
                                    layers.begin(), layers.end(),
                                    [&](auto const& L) { return std::strcmp(L.layerName, ln) == 0; }
                                );
                                if (lit != layers.end()) {
                                    return { *lit, *eit };
                                }
                            }
                            // fallback layer
                            return { SelectLayer(layerPref), *eit };
                        }
                    }

                    // 3) Full generic fallback
                    return { SelectLayer(layerPref), SelectExtension(nullptr, extPref) };
                }


            }
        }
    }
}