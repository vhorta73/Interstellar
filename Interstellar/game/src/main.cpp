#include <iostream>

#include "Interstellar/Interstellar.hpp"

//using namespace Interstellar::Core;

int main() {

    // Create the game instance.
    Interstellar::Game game;

    // Load game configuration: e.g., window settings, controls etc.
    try {
        game.loadConfig();
    }
    catch (const std::exception& e) {
        std::cerr << "LoadConfig failed with: " << e.what();
    }

    // Build all game components.
    try {
        game.buildComponents();
    }
    catch (const std::exception& e) {
        std::cerr << "Build Components failed wiht: " << e.what();
    }

    // Play
    try {
        game.start();
    }
    catch (const std::exception& e) {
        std::cerr << "Game start failed with: " << e.what();
    }

    return 0;
}