#include <iostream>

#include "Interstellar/Interstellar.hpp"
#include "Interstellar/Core/Logging.hpp"

int main() {

    // Prepare a generic logger.
    const auto genericLogger = Interstellar::Core::Logger();
    
    // Create the game instance.
    Interstellar::Game game;

    // Load game configuration: e.g., window settings, controls etc.
    try {
        game.loadConfig();
    }
    catch (const std::exception& e) {
        genericLogger.LogCritical(std::string("LoadConfig: ") + e.what());
        return 1; // Config error.
    }

    // Build all game components.
    try {
        game.buildComponents();
    }
    catch (const std::exception& e) {
        genericLogger.LogCritical(std::string("BuildComponents: ") + e.what());
        return 2; // Building error.
    }

    // Play
    try {
        game.start();
    }
    catch (const std::exception& e) {
        genericLogger.LogCritical(std::string("Game start: ") + e.what());
        return 3; // Starting error.
    }

    try {
        game.shutdown();
    }
    catch (const std::exception& e) {
        genericLogger.LogCritical(std::string("Game shutdown: ") + e.what());
        return 4; // Shutdown error.
    }

    return 0;
}