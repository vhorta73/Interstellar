#include "Interstellar/Core/Logging.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "Interstellar/Core/LogLevel.hpp"

namespace Interstellar::Core {

    spdlog::level::level_enum ToSpdLevel(LogLevel lvl) {
        using enum spdlog::level::level_enum;
        switch (lvl) {
        case LogLevel::Trace:    return trace;
        case LogLevel::Debug:    return debug;
        case LogLevel::Info:     return info;
        case LogLevel::Warn:     return warn;
        case LogLevel::Error:    return err;
        case LogLevel::Critical: return critical;
        default:                 return info;
        }
    }

    // Default Logger instantiation with GENERIC_LOG.
    Logger::Logger() : Logger(GENERIC_LOG) {}

    // Accepts any of the Logging available constants.
    Logger::Logger(const std::string& loggerName) {
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::info);

        m_Logger = std::make_shared<spdlog::logger>(loggerName, consoleSink);
        m_Logger->set_level(spdlog::level::info);

        spdlog::register_logger(m_Logger);
        spdlog::set_default_logger(m_Logger);
        spdlog::flush_on(spdlog::level::err);
        if (!m_Logger) {
            throw std::runtime_error("Logger with name '" + loggerName + "' not found.");
        }
    }

    // Convenience wrappers
    void Logger::LogInfo(const std::string& msg) { m_Logger->info(msg); }
    void Logger::LogWarn(const std::string& msg) { m_Logger->warn(msg); }
    void Logger::LogError(const std::string& msg) { m_Logger->error(msg); }
    void Logger::LogDebug(const std::string& msg) { m_Logger->debug(msg); }
    void Logger::LogCritical(const std::string& msg) { m_Logger->critical(msg); }

}