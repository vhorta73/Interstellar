#pragma once
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include "Interstellar/Graphics/Vulkan/Selectors/VulkanSelector.hpp"

namespace TestHelpers {

    // Pure C++ helper for testing and hard coded combo tables
    inline std::pair<std::string, std::string>
        ComputePreferredCombo(
            const std::vector<std::string>& availableLayers,
            const std::vector<std::string>& availableExts)
    {
        using namespace Interstellar::Graphics::Vulkan::Selectors;

        // 1) Layer first table
        for (auto const& combo : PreferredLayerExtensionCombos()) {
            const std::string layerName = combo.first;
            const auto& extPrefs = combo.second;

            if (std::find(availableLayers.begin(), availableLayers.end(), layerName)
                != availableLayers.end())
            {
                // pick the highest priority extension that’s present
                for (auto const& extNameC : extPrefs) {
                    const std::string extName = extNameC;
                    if (std::find(availableExts.begin(), availableExts.end(), extName)
                        != availableExts.end())
                    {
                        return std::make_pair(layerName, extName);
                    }
                }
                // fallback: first available extension
                if (!availableExts.empty())
                    return std::make_pair(layerName, availableExts.front());
            }
        }

        // 2) Extension first table
        for (auto const& combo : PreferredExtensionLayerCombos()) {
            const std::string extName = combo.first;
            const auto& layerPrefs = combo.second;

            if (std::find(availableExts.begin(), availableExts.end(), extName)
                != availableExts.end())
            {
                for (auto const& layerNameC : layerPrefs) {
                    const std::string layerName = layerNameC;
                    if (std::find(availableLayers.begin(), availableLayers.end(), layerName)
                        != availableLayers.end())
                    {
                        return std::make_pair(layerName, extName);
                    }
                }
                // fallback: first available layer
                if (!availableLayers.empty())
                    return std::make_pair(availableLayers.front(), extName);
            }
        }

        // 3) Generic fallback
        const std::string fallbackLayer =
            availableLayers.empty() ? std::string{} : availableLayers.front();

        const std::string fallbackExt =
            availableExts.empty() ? std::string{} : availableExts.front();

        return std::make_pair(fallbackLayer, fallbackExt);
    };
}