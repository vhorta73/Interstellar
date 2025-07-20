//#include <iostream>
//#include "Interstellar/Graphics/Vulkan/Selectors/VulkanSelector.hpp"
//#include <vulkan/vulkan.h>  
//
//int main(int argc, char* argv[]) {
//    std::cout << "Starting Interstellar...\n";
//    auto [ layer, extension] = Interstellar::Graphics::Vulkan::Selectors::SelectLayerAndExtensionCombo();
//    std::cout << "Selected Layer: " << layer.layerName << "\n";
//    std::cout << "Selected Extension: " << extension.extensionName << "\n";
//
//
//    //VkInstance instance = createVulkanInstance(layers, exts);
//    //if (instance == VK_NULL_HANDLE) {
//        //return EXIT_FAILURE;
//    //}
//
//    return 0;
//}



//#include "Core/GameGraphicsBuilder.hpp"
//#include "Core/Window.hpp"
//#include "Core/ILogging.hpp"
//#include "Core/IGame.hpp"
//#include "Vulkan/VulkanBuilderImpl.hpp"
//#include "MyGame/MyGame.hpp"          // your concrete game class
#include <exception>
#include <iostream>
//
//using namespace Interstellar::Graphics::Core;
//using namespace Interstellar::Platform;
//using namespace Interstellar::Game;
//
int main(int argc, char** argv)
{
        std::cout << "Starting Interstellar...\n";
        try
    {
//        // 1) Parse command-line / config
//        AppConfig config = AppConfig::LoadFromArgs(argc, argv);
//
//        // 2) Initialize logging (console, file, verbosity)
//        Logging::Init(config.logLevel, config.logFilePath);
//        LOG_INFO("Starting Interstellar Engine");
//
//        // 3) Create your window/context
//        WindowDesc wndDesc{ config.windowTitle, config.windowWidth, config.windowHeight };
//        Window window(wndDesc);
//
//        // 4) Build the graphics device via the Game-facing builder
//        //    (this uses your GameGraphicsBuilder + VulkanBuilderImpl)
//        ContextCreateInfo vkCi{/*...from config...*/ };
//        auto vkSelector = std::make_shared<YourExtensionSelector>(/*...*/);
//
//        auto graphicsDevice = GameGraphicsBuilder{}
//            .UseBackend(std::make_unique<VulkanBuilderImpl>(vkCi, vkSelector))
//            .Build();
//
//        // 5) Instantiate your Game
//        std::unique_ptr<IGame> game = std::make_unique<MyGame>(graphicsDevice, window);
//
//        // 6) Lifecycle: init, run loop, shutdown
//        game->Initialize();
//
//        // Main loop
//        Timer    frameTimer;
//        while (window.PollEvents() && !game->ShouldExit())
//        {
//            float delta = frameTimer.Restart();
//
//            game->Update(delta);
//            graphicsDevice->BeginFrame();
//            game->Render();
//            graphicsDevice->EndFrame();
//        }
//
//        game->Shutdown();
//        LOG_INFO("Shutting down cleanly");
        return 0;
    }
    catch (const std::exception& e)
    {
        // Catch-all: log and return error
//        LOG_CRITICAL("Fatal error: {}", e.what());
        return -1;
    }
}
