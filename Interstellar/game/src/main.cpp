#include <iostream>

#include "Interstellar/Interstellar.hpp"
#include "Interstellar/Core/Logging.hpp"

int main() {

    // Prepare a generic logger.
    auto genericLogger = Interstellar::Core::Logger();
    
    // Create the game instance.
    Interstellar::Game game;

    // Load game configuration: e.g., window settings, controls etc.
    try {
        game.loadConfig();
    }
    catch (const std::exception& e) {
        genericLogger.LogError(std::string("LoadConfig: ") + e.what());
        return 1;
    }

    // Build all game components.
    try {
        game.buildComponents();
    }
    catch (const std::exception& e) {
        genericLogger.LogError(std::string("BuildComponents: ") + e.what());
        return 1;
    }

    // Play
    try {
        game.start();
    }
    catch (const std::exception& e) {
        genericLogger.LogError(std::string("Game start: ") + e.what());
        return 1;
    }

    return 0;
}