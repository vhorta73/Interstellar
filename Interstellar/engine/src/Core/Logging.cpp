#pragma once

#include "Interstellar/Core/Logging.hpp"
#include <spdlog/spdlog.h> 
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Interstellar::Core {

    spdlog::level::level_enum ToSpdLevel(LogLevel lvl)
    {
        switch (lvl) {
        case LogLevel::Trace:    return spdlog::level::trace;
        case LogLevel::Debug:    return spdlog::level::debug;
        case LogLevel::Info:     return spdlog::level::info;
        case LogLevel::Warn:     return spdlog::level::warn;
        case LogLevel::Error:    return spdlog::level::err;
        case LogLevel::Critical: return spdlog::level::critical;
        default:                 return spdlog::level::info;
        }
    }

    void Init(AppConfig const& cfg)
    {
        std::vector<spdlog::sink_ptr> sinks;

        // Console sink with color
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(ToSpdLevel(cfg.logLevel));
        sinks.push_back(consoleSink);

        // Optional file sink
        if (!cfg.logFilePath.empty()) {
            auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                cfg.logFilePath, /*truncate=*/false);
            fileSink->set_level(ToSpdLevel(cfg.logLevel));
            sinks.push_back(fileSink);
        }

        // Create a default logger combining sinks
        auto logger = std::make_shared<spdlog::logger>("interstellar", begin(sinks), end(sinks));
        logger->set_level(ToSpdLevel(cfg.logLevel));
        spdlog::set_default_logger(logger);

        // Optional: flush every message of Error or higher
        spdlog::flush_on(spdlog::level::err);
    }
}
