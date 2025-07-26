#pragma once

#include "Interstellar/Config/JsonImpl/UserInfoConfig.hpp"
#include <string>
#include <cstring>


// This file is part of the Interstellar game engine.
// It defines the Game class and a Builder class for constructing Game objects.
namespace Interstellar {

    // Game class representing a game object.
    class GameOld {
    public:
        std::string& getText() { return text; }
        std::unique_ptr<IJsonConfig>& getUserConfig() { return userConfig; }

    private:
        std::string text;
        std::unique_ptr<IJsonConfig> userConfig;
        friend class Builder; // Allow Builder to access private members
    };

    // Builder class for constructing Game objects.
    //class Builder {
    //public:
        //Builder& setText(const std::string& txt) {
            //text = txt;
            //return *this;
        //};
        //Builder& setUserConfig(std::unique_ptr<IJsonConfig> config) {
            //userConfig = std::move(config);
            //return *this;
        //};

        //Game build() {
            //Game game;
            //if ( text.size() == 0 ) {
                //std::cerr << "Warning: No text set for the game." << std::endl;
                //std::abort();
            //}
            //game.text = this->text;
            //game.userConfig = std::move(this->userConfig);
            //return game;
        //}

    //private:
        //std::string text;
        //std::unique_ptr<IJsonConfig> userConfig;
    //};

}