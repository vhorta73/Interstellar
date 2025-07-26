#include <iostream>

#include "Interstellar/Interstellar.hpp"

using namespace Interstellar::Core;

int main() {

    // Create the game instance.
    Interstellar::Game game;

    // Load game configuration: e.g., window settings, controls etc.
    game.loadConfig();

    // Build all game components.
    game.buildComponents();

    // Play
    game.start();

    return 0;
}