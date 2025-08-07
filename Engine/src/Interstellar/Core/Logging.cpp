#include "Interstellar/Core/Logging.hpp"
#include "Interstellar/Core/LogLevel.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

#include <filesystem>

// Set global flush policy before any logger is used
static const auto _flushPolicy = []() {
    spdlog::flush_on(spdlog::level::err);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%l%$] %v"); // Format: [date time] [logger] [LEVEL] message
    return 0;
}();

namespace Interstellar::Core {

    static spdlog::level::level_enum ToSpdLevel(LogLevel lvl) {
        using enum spdlog::level::level_enum;
        switch (lvl) {
        case LogLevel::Critical: return critical;
        case LogLevel::Error:    return err;
        case LogLevel::Warn:     return warn;
        case LogLevel::Info:     return info;
        case LogLevel::Debug:    return debug;
        case LogLevel::Trace:    return trace;
        default:                 return info;
        }
    }

    static LogLevel GetDefaultLevelForLogger(const std::string& loggerName) {
        if (loggerName == LOG_GRAPHIC)  return LogLevel::Info;
        if (loggerName == LOG_CONFIG)   return LogLevel::Debug;
        if (loggerName == LOG_GENERIC)  return LogLevel::Critical;
        if (loggerName == LOG_INIT)     return LogLevel::Info;

        return LogLevel::Info; // fallback
    }

    // Default Logger instantiation with GENERIC_LOG.
    Logger::Logger()                                              : Logger(LOG_GENERIC, GetDefaultLevelForLogger(LOG_GENERIC)) {}
    Logger::Logger(LogLevel level)                                : Logger(LOG_GENERIC, level) {}
    Logger::Logger(const std::string& loggerName)                 : Logger(loggerName, GetDefaultLevelForLogger(loggerName)) {}

    // Accepts any of the Logging available constants.
    Logger::Logger(const std::string& loggerName, LogLevel level) {
        auto spdLevel = ToSpdLevel(level);
        m_Logger = spdlog::get(loggerName);

        if (!m_Logger) {
            std::filesystem::create_directories("logs");

            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto fileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
                "logs/" + loggerName + ".log", 0, 0  // Rotate at 00:00
            );

            std::vector<spdlog::sink_ptr> sinks{ consoleSink, fileSink };
            m_Logger = std::make_shared<spdlog::logger>(loggerName, sinks.begin(), sinks.end());
            m_Logger->set_level(spdLevel);
            m_Logger->flush_on(spdlog::level::err);
            spdlog::register_logger(m_Logger);
        }

        // Only set default if this is the generic logger
        if (loggerName == LOG_GENERIC) {
            spdlog::set_default_logger(m_Logger);
        }
    }

    // Convenience wrappers
    void Logger::LogCritical(const std::string& msg) const { m_Logger->critical(msg); }
    void Logger::LogError(const std::string& msg) const { m_Logger->error(msg); }
    void Logger::LogWarn(const std::string& msg) const { m_Logger->warn(msg); }
    void Logger::LogDebug(const std::string& msg) const { m_Logger->debug(msg); }
    void Logger::LogInfo(const std::string& msg) const { m_Logger->info(msg); }
    void Logger::LogTrace(const std::string& msg) const { m_Logger->trace(msg); }
}
