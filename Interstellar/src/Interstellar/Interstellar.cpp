#include <iostream>

#include "Interstellar/Interstellar.hpp"
#include "Interstellar/Core/Logging.hpp"

namespace Interstellar {

    static const Core::Logger s_Logger(Core::LOG_INIT);

    Game::Game() {
        s_Logger.LogDebug("Game constructed.");
    }

    Game::~Game() {
        s_Logger.LogDebug("Game destructed.");
    }

    void Game::loadConfig() {
        s_Logger.LogInfo("Loading config...");
        // Load your config here.
    }

    void Game::buildComponents() {
        s_Logger.LogInfo("Building components...");
        // Setup systems.
    }

    void Game::start() {
        s_Logger.LogInfo("Game started!");
        // Main loop.
    }

    void Game::shutdown() {
        s_Logger.LogInfo("Game shutting down!");
        // Shutting down.
    }

}
