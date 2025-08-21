#include <iostream>

#include "Interstellar/Interstellar.hpp"
#include "Interstellar/Logging/Logging.hpp"

namespace Interstellar {

    using Interstellar::Logging::LogInit;

    Game::Game() {
        LogInit().LogDebug("Game constructed.");
    }

    Game::~Game() {
        LogInit().LogDebug("Game destructed.");
    }

    void Game::loadConfig() {
        LogInit().LogInfo("Loading config...");
        // Load your config here.
    }

    void Game::buildComponents() {
        LogInit().LogInfo("Building components...");
        // Setup systems.
    }

    void Game::start() {
        LogInit().LogInfo("Game started!");
        // Main loop.
    }

    void Game::shutdown() {
        LogInit().LogInfo("Game shutting down!");
        // Shutting down.
    }

}
