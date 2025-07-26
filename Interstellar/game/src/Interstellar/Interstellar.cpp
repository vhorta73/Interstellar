#include "Interstellar/Interstellar.hpp"
#include <iostream>

namespace Interstellar {

    Game::Game() {
        std::cout << "Game constructed\n";
    }

    Game::~Game() {
        std::cout << "Game destructed\n";
    }

    void Game::loadConfig() {
        std::cout << "Loading config...\n";
        // Load your config here.
    }

    void Game::buildComponents() {
        std::cout << "Building components...\n";
        // Setup systems.
    }

    void Game::start() {
        std::cout << "Game started!\n";
        // Main loop.
    }

    void Game::shutdown() {
        std::cout << "Game shutting down!\n";
        // Shutting down.
    }

}
