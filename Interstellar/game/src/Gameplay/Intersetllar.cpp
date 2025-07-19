#include <iostream>
#include "Graphics/Vulkan/Selectors/VulkanSelector.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Starting Interstellar...\n";
    auto layer = Graphics::Vulkan::Selectors::SelectLayerWithFallback();
    auto extension = Graphics::Vulkan::Selectors::SelectExtensionWithFallback();
    auto [clayer, cextension] = Graphics::Vulkan::Selectors::SelectLayerAndExtensionCombo();
    std::cout << "Selected Layer: " << layer.layerName << "\n";
    std::cout << "Selected Extension: " << extension.extensionName << "\n";
    std::cout << "Combo leys: " << clayer.layerName << " + " << cextension.extensionName << "\n";

    return 0;
}
