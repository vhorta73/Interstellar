#pragma once

#include <string>

#include "Interstellar/Core/LogLevelUtils.hpp"

namespace Interstellar::Core
{
    /// Holds all startup configuration, loaded from a JSON file and overridden by CLI.
    struct AppConfig
    {
        // Path to JSON config file (set via --config-file, defaults if empty)
        std::string configFilePath;

        // Command-line / config options
        LogLevel    logLevel = LogLevel::Info;
        std::string logFilePath;
        std::string windowTitle = "Interstellar";
        int         windowWidth = 1280;
        int         windowHeight = 720;
        bool        enableVulkanValidation = false;

        /// Load defaults, then JSON config file, then override with CLI args.
        /// Throws CLI::ParseError on bad arguments, std::runtime_error on malformed JSON.
        static AppConfig LoadFromArgs(int argc, char** argv);
    };
}
