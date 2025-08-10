#include "Interstellar/Core/AppConfig.hpp"
#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <map>
#include <algorithm>

#include "Interstellar/Core/LogLevelUtils.hpp"
namespace Interstellar::Core {

    AppConfig AppConfig::LoadFromArgs(int argc, char** argv)
    {
        AppConfig config;

        //
        // Phase 1: pick up --config-file (allow extras so other flags don't error)
        //
        {
            CLI::App cfgApp("Interstellar Config Loader");

            cfgApp.add_option("-c,--config-file", config.configFilePath,
                "Path to JSON config file");

            cfgApp.allow_extras();
            cfgApp.parse(argc, argv);
        }

        // Default config path if non provided
        if ( config.configFilePath.empty() )
        {
            config.configFilePath = "assets/config/InterstellarConfig.json";
        }

        //
        // Phase 2: load JSON config file
        //
        if (std::filesystem::exists(config.configFilePath)) {
            std::ifstream file(config.configFilePath);
            if (!file.is_open())
                throw std::runtime_error("Failed to open config file: " + config.configFilePath);

            nlohmann::json json;
            file >> json;

            // Load JSON values into config
            if (json.contains("logLevel"))
                config.logLevel = ParseLogLevel(json["logLevel"].get<std::string>());

            if (json.contains("logFilePath"))
                config.logFilePath = json["logFilePath"].get<std::string>();

            if (json.contains("windowTitle"))
                config.windowTitle = json["windowTitle"].get<std::string>();

            if (json.contains("windowWidth"))
                config.windowWidth = json["windowWidth"].get<int>();

            if (json.contains("windowHeight"))
                config.windowHeight = json["windowHeight"].get<int>();

            if (json.contains("enableVulkanValidation"))
                config.enableVulkanValidation = json["enableVulkanValidation"].get<bool>();

        }
        else {
            namespace fs = std::filesystem;
            throw std::runtime_error("Config file does not exist: " + config.configFilePath + " in: " + fs::current_path().string() + "\n");
        }
        // else: no JSON file -> keep defaults

        //
        // Phase 3: override with command-line arguments
        //
        {
            CLI::App app{ "Interstellar Application Configuration" };
            app.add_option("-l,--log-level", config.logLevel, "Logging verbosity level")
                ->capture_default_str()
                ->transform(CLI::CheckedTransformer(
                    std::map<std::string, LogLevel>{
                        {"trace", LogLevel::Trace},
                        {"debug", LogLevel::Debug},
                        {"info", LogLevel::Info},
                        {"warn", LogLevel::Warn},
                        {"error", LogLevel::Error},
                        {"critical", LogLevel::Critical}
                    }, CLI::ignore_case
                ));

            app.add_option("-f,--log-file", config.logFilePath,
                "Path to log file (empty for console only)");

            app.parse(argc, argv);
        }

        return config;
    }

}
