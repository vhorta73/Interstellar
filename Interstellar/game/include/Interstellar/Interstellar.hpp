#pragma once

#include <memory>

namespace Interstellar {

    class Game {
    public:
        Game();
        ~Game();

        // Load configuration logic here
        void loadConfig();

        // Build game components logic here
        void buildComponents();

        // Start the game logic here
        void start();

        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;

    private:
        struct Implementation;
        std::unique_ptr<Implementation> implementation;
    };
}