#pragma once

#include <memory>

namespace Interstellar {

    /**
     * @brief Central class for managing the game lifecycle.
     *
     * Responsible for loading configuration, building components,
     * starting the main game loop, and shutting down gracefully.
     */
    class Game {
    public:
        /**
         * @brief Constructs the game instance.
         */
        Game();

        /**
         * @brief Cleans up all game resources and components.
         */
        ~Game();

        /**
         * @brief Loads game configuration (from files or defaults).
         */
        void loadConfig();

        /**
         * @brief Builds core systems and gameplay components.
         */
        void buildComponents();

        /**
         * @brief Starts the main game loop or runtime logic.
         */
        void start();

        /**
         * @brief Shuts down the game and cleans up resources.
         */
        void shutdown();


        Game(const Game&) = delete;

        Game& operator = (const Game&) = delete;
    };
}