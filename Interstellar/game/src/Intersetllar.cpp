#include <iostream>

#include <memory>
#include <string>
#include "Interstellar/Interstellar.hpp"

namespace Interstellar {

    struct Game::Implementation {
        std::string text;

        struct Settings {
            int test = 0;
        } settings;

        // Add any private members or methods here
        Implementation() {
            std::cout << "Game Implementation created.\n";
        }
        
        ~Implementation() {
            std::cout << "Game Implementation destroyed.\n";
        }
    };


    Game::Game() : implementation(std::make_unique<Implementation>()) { };

    Game::~Game() = default;

    void Game::loadConfig() {
        throw std::runtime_error("TODO");
        // Load configuration logic here
        implementation->text = "Loaded config";
            
        std::cout << "Loading game configuration... " << implementation->text << "\n";
        //<< implementation->text << std::endl;
    }

    void Game::buildComponents() {
        throw std::runtime_error("TODO");
        // Build game components logic here
        //implementation->settings.test = 3;
        std::cout << "Building game components... " << implementation->text << "\n";
    }

    void Game::start() {
        throw std::runtime_error("TODO");
        // Start the game logic here
        std::cout << "Starting the game...\n";
        //std::cout << "Game Implementation created.\n" << implementation->settings.test;
    }
};
