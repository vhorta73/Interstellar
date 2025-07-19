#include <iostream>
#include "Graphics/Vulkan/VulkanSelector.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Starting Interstellar...\n";
    auto layer = Graphics::Vulkan::SelectLayer();
    auto extension = Graphics::Vulkan::SelectExtension();
    std::cout << "Selected Layer: " << layer.layerName << "\n";
    std::cout << "Selected Extension: " << extension.extensionName << "\n";
    std::cin.get();
    return 0;
}
