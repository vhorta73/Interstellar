#include "pch.h"
#include "CppUnitTest.h"
#include <vector>
#include "VulkanSelectorTestHelpers.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace VulkanLayerSelectorTest
{
    TEST_CLASS(ComboTableTests)
    {
    public:
        TEST_METHOD(LayerFirst_PicksTopPair)
        {
             const std::vector<std::string> layers = {
                "VK_LAYER_KHRONOS_validation",
                "VK_LAYER_LUNARG_gfxreconstruct"
            };
            static const std::vector<std::string> exts = {
                "VK_EXT_debug_utils",
                "VK_KHR_surface"
            };

            auto layerExtensionPair = TestHelpers::ComputePreferredCombo(layers, exts);
            Assert::AreEqual(std::string("VK_LAYER_KHRONOS_validation"), layerExtensionPair.first);
            Assert::AreEqual(std::string("VK_EXT_debug_utils"), layerExtensionPair.second);
        }

        TEST_METHOD(LayerFirst_FallbackToNextExtension)
        {
            // Imagine validation layer present but only VK_EXT_debug_report is available
            static const std::vector<std::string> layers = {
                "VK_LAYER_KHRONOS_validation"
            };
            static const std::vector<std::string> exts = {
                "VK_EXT_debug_report",    // lower priority than debug_utils
                "VK_KHR_surface"
            };

            auto layerExtensionPair = TestHelpers::ComputePreferredCombo(layers, exts);
            Assert::AreEqual(std::string("VK_LAYER_KHRONOS_validation"), layerExtensionPair.first);
            Assert::AreEqual(std::string("VK_EXT_debug_report"), layerExtensionPair.second);
        }

        TEST_METHOD(LayerFirst_FallbackToFirstAvailableExtension)
        {
            // validation present, but none of its preferred exts are available
            static const std::vector<std::string> layers = {
                "VK_LAYER_KHRONOS_validation"
            };
            static const std::vector<std::string> exts = {
                "VK_KHR_surface",
                "VK_EXT_unknown"   // not in any table
            };

            auto layerExtensionPair = TestHelpers::ComputePreferredCombo(layers, exts);
            Assert::AreEqual(std::string("VK_LAYER_KHRONOS_validation"), layerExtensionPair.first);
            // fallback picks first in exts vector
            Assert::AreEqual(std::string("VK_KHR_surface"), layerExtensionPair.second);
        }

        TEST_METHOD(ExtensionFirst_PicksTopPair)
        {
            // No known layers present, but a known extension with its layer list
            static const std::vector<std::string> layers = {
                "Some_Other_Layer"
            };
            static const std::vector<std::string> exts = {
                "VK_EXT_debug_utils",     // maps to VK_LAYER_KHRONOS_validation
                "VK_KHR_surface"
            };

            auto layerExtensionPair = TestHelpers::ComputePreferredCombo(layers, exts);
            const std::string layerName = layerExtensionPair.first;
            const auto& extPrefs = layerExtensionPair.second;

            Assert::AreEqual(std::string("Some_Other_Layer"), layerName);
            Assert::AreEqual(std::string("VK_EXT_debug_utils"), extPrefs);
        }

        TEST_METHOD(GenericFallback_WhenNoTableMatches)
        {
            // Neither known layers nor known extensions
            static const std::vector<std::string> layers = {
                "LAYER_A", "LAYER_B"
            };
            static const std::vector<std::string> exts = {
                "EXT_X", "EXT_Y"
            };

            auto layerExtensionPair = TestHelpers::ComputePreferredCombo(layers, exts);
            // picks first of each list
            Assert::AreEqual(std::string("LAYER_A"), layerExtensionPair.first);
            Assert::AreEqual(std::string("EXT_X"), layerExtensionPair.second);
        }
    };
}
