#include <exception>
#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>
#include "Interstellar/Config/AppConfig.hpp"
#include "Interstellar/Config/JsonImpl/UserInfoConfig.hpp"

int main(int argc, char* argv[])
{
    std::cout << "Starting Interstellar...\n";
    try
    {
        // 1) Parse command-line / config
        std::cout << "AppConfig - Loading...\n";
        auto appConfig = AppConfig::New();
        //appConfig->save(new UserInfoConfig{ "Test", 35 });
        auto info = appConfig->load<UserInfoConfig>();
        std::cout << "Loaded user info: "
                  << "Name=" << info->Name
                  << ", Age=" << info->Age
            << "\n";
        std::cout << "AppConfig - Loaded.\n";
        //std::cout << "Loaded config: "
                  //<< "LogLevel=" << static_cast<int>(config->logLevel)
                  //<< ", Window=" << config->windowTitle
                  //<< "(" << config->windowWidth << "x" << config->windowHeight << ")"
                  //<< ", LogFile=" << (config->logFilePath.empty() ? "none" : config->logFilePath)
            //<< "\n";

        //auto config = AppConfig::LoadFromArgs(argc, argv);
        //std::cout << "AppConfig - Loaded.\n";
        //std::cout << "Loaded config: "
                  //<< "LogLevel=" << static_cast<int>(config.logLevel)
                  //<< ", Window=" << config.windowTitle
                  //<< "(" << config.windowWidth << "x" << config.windowHeight << ")"
                  //<< ", LogFile=" << (config.logFilePath.empty() ? "none" : config.logFilePath)
            //<< "\n";

        //Logging::Init(config);
        //spdlog::info("Starting Interstellar Engine ({}, {}x{})",
            //config.windowTitle, config.windowWidth, config.windowHeight);
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
        SPDLOG_INFO("Shutting down cleanly");
        return 0;
    }
    catch (const std::exception& e)
    {
        // Catch-all: log and return error
        SPDLOG_CRITICAL("Fatal error: {}", e.what());
        return -1;
    }
}
