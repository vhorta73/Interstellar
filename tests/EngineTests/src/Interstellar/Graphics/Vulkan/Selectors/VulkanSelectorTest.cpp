#include <gtest/gtest.h>
#include <vector>
#include "Helpers/VulkanSelectorTestHelpers.hpp"

namespace Interstellar_Graphics_Vulkan_Selectors_Test {

    using TestHelpers::ComputePreferredCombo;

    using PreferencePair = std::pair<std::string, std::string>;

    class VulkanSelectorTest : public ::testing::Test {
    protected:
        std::vector<std::string> layers;
        std::vector<std::string> exts;
        PreferencePair result;

        void Compute() {
            result = ComputePreferredCombo(layers, exts);
        }
    };

    /// @brief When both validation and gfxreconstruct layers are available,
    /// pick the highest-priority layer and its top preferred extension.
    TEST_F(VulkanSelectorTest, LayerFirst_PicksTopPair) {
        layers = { "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_gfxreconstruct" };
        exts = { "VK_EXT_debug_utils", "VK_KHR_surface" };
        Compute();

        EXPECT_EQ("VK_LAYER_KHRONOS_validation", result.first);
        EXPECT_EQ("VK_EXT_debug_utils", result.second);
    }

    /// @brief If only debug_report extension is available, fallback from debug_utils -> debug_report.
    TEST_F(VulkanSelectorTest, LayerFirst_FallbacksToNextExtension) {
        layers = { "VK_LAYER_KHRONOS_validation" };
        exts = { "VK_EXT_debug_report", "VK_KHR_surface" };
        Compute();

        EXPECT_EQ("VK_LAYER_KHRONOS_validation", result.first);
        EXPECT_EQ("VK_EXT_debug_report", result.second);
    }

    /// @brief If none of the preferred extensions are present, pick the first available extension.
    TEST_F(VulkanSelectorTest, LayerFirst_FallbacksToFirstAvailableExtension) {
        layers = { "VK_LAYER_KHRONOS_validation" };
        exts = { "VK_KHR_surface", "VK_EXT_unknown" };
        Compute();

        EXPECT_EQ("VK_LAYER_KHRONOS_validation", result.first);
        EXPECT_EQ("VK_KHR_surface", result.second);
    }

    /// @brief With an unknown layer but known extension, keep the provided layer and pick the preferred extension.
    TEST_F(VulkanSelectorTest, ExtensionFirst_PicksTopPair) {
        layers = { "Some_Other_Layer" };
        exts = { "VK_EXT_debug_utils", "VK_KHR_surface" };
        Compute();

        EXPECT_EQ("Some_Other_Layer", result.first);
        EXPECT_EQ("VK_EXT_debug_utils", result.second);
    }

    /// @brief When no known layers or extensions match preferences, fall back to the first of each list.
    TEST_F(VulkanSelectorTest, GenericFallback_WhenNoTableMatches) {
        layers = { "LAYER_A", "LAYER_B" };
        exts = { "EXT_X", "EXT_Y" };
        Compute();

        EXPECT_EQ("LAYER_A", result.first);
        EXPECT_EQ("EXT_X", result.second);
    }

    /// @brief Edge case: empty layers or extensions should produce empty strings in the result.
    TEST_F(VulkanSelectorTest, EmptyInputs_YieldsEmptyStrings) {
        layers.clear();
        exts.clear();
        Compute();

        EXPECT_EQ("", result.first);
        EXPECT_EQ("", result.second);
    }

}  // namespace Interstellar_Graphics_Vulkan_Selectors_Test
