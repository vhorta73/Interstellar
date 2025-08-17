/**
 * @file
 * @brief Implementation of the Interstellar logging facade.
 * @details
 * Initializes spdlog's global policy once per process:
 *  - flush_on(err)
 *  - flush_every(2s)  // spawns a lightweight background flusher thread
 *  - uniform output pattern
 * and materializes the four global logger instances.
 *
 * @env INTERSTELLAR_LOG_DIR
 *      If set, log files will be created under this directory.
 *      Otherwise logs are written under the working directory in `./logs`.
 *
 * @thread_safety
 * All logging calls are thread-safe (multithreaded sinks).
 *
 * @note If the file sink cannot be created (e.g., permission denied),
 *       logging silently falls back to console-only.
 */

#include "Interstellar/Logging/Logging.hpp"
#include "Interstellar/Logging/LogLevel.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

#include <chrono>
#include <filesystem>
#include <mutex>
#include <vector>
#include <optional>
#include <cstdlib>     // getenv/_dupenv_s
#include <string>
#include <string_view>

namespace {

    // Unity/Jumbo-safe: initialize global spdlog policy exactly once
    std::once_flag g_spdlog_policy_flag;

    void EnsureSpdlogGlobalPolicyInitialized() {
        std::call_once(g_spdlog_policy_flag, [] {
            spdlog::flush_on(spdlog::level::err);
            spdlog::flush_every(std::chrono::seconds(2));
            spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%l%$] %v");
            });
    }

    // Safe env getter (avoids MSVC C4996 warning)
    std::optional<std::string> GetEnv(const char* name) {
#ifdef _WIN32
        size_t len = 0;
        char* buf = nullptr;
        if (_dupenv_s(&buf, &len, name) == 0 && buf) {
            std::string v(buf);
            free(buf);
            if (!v.empty()) return v;
        }
        return std::nullopt;
#else
        if (const char* v = std::getenv(name); v && *v) return std::string(v);
        return std::nullopt;
#endif
    }

    inline std::filesystem::path LogsDir() {
        if (auto v = GetEnv("INTERSTELLAR_LOG_DIR")) return std::filesystem::path(*v);
        return std::filesystem::path("logs"); // relative to working dir
    }

} // namespace

namespace Interstellar::Logging {

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

    static LogLevel GetDefaultLevelForLogger(std::string_view loggerName) {
        if (loggerName == LOG_GRAPHIC)  return LogLevel::Info;
        if (loggerName == LOG_CONFIG)   return LogLevel::Debug;
        if (loggerName == LOG_GENERIC)  return LogLevel::Critical;
        if (loggerName == LOG_INIT)     return LogLevel::Info;
        return LogLevel::Info; // fallback
    }

    // Default Logger instantiation with GENERIC_LOG.
    Logger::Logger() : Logger(LOG_GENERIC, GetDefaultLevelForLogger(LOG_GENERIC)) {}
    Logger::Logger(LogLevel level) : Logger(LOG_GENERIC, level) {}
    Logger::Logger(std::string_view name) : Logger(name, GetDefaultLevelForLogger(name)) {}

    Logger::Logger(std::string_view loggerName, LogLevel level) {
        EnsureSpdlogGlobalPolicyInitialized();

        const std::string nameStr{ loggerName };
        const auto spdLevel = ToSpdLevel(level);

        m_Logger = spdlog::get(nameStr);
        if (!m_Logger) {
            // Ensure directory exists (respect env override); don't throw on failure
            const auto dir = LogsDir();
            std::error_code ec;
            std::filesystem::create_directories(dir, ec);

            // Always have console sink
            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            std::vector<spdlog::sink_ptr> sinks;
            sinks.push_back(consoleSink);

            // Try to add daily file sink; if it fails, keep console-only
            try {
                const auto filePath = (dir / (nameStr + ".log")).string();
                auto fileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(filePath, 0, 0);
                sinks.push_back(fileSink);
            }
            catch (...) {
                // Optional: one-off diagnostic could be printed to stderr if desired.
            }

            m_Logger = std::make_shared<spdlog::logger>(nameStr, sinks.begin(), sinks.end());
            m_Logger->set_level(spdLevel);
            spdlog::register_logger(m_Logger);
        }
        else {
            // Predictable behavior: adopt the level requested by the wrapper
            m_Logger->set_level(spdLevel);
        }

        if (loggerName == LOG_GENERIC) {
            spdlog::set_default_logger(m_Logger);
        }
    }

    Logger::~Logger() noexcept {
        if (m_Logger) {
            try { m_Logger->flush(); }
            catch (...) { /* honor noexcept */ }
        }
    }

    // Unformatted wrappers
    void Logger::LogCritical(const std::string& msg) const { m_Logger->critical(msg); }
    void Logger::LogError(const std::string& msg) const { m_Logger->error(msg); }
    void Logger::LogWarn(const std::string& msg) const { m_Logger->warn(msg); }
    void Logger::LogDebug(const std::string& msg) const { m_Logger->debug(msg); }
    void Logger::LogInfo(const std::string& msg) const { m_Logger->info(msg); }
    void Logger::LogTrace(const std::string& msg) const { m_Logger->trace(msg); }

    // Single definitions for the globals
    const Logger logGeneric{ LOG_GENERIC, GetDefaultLevelForLogger(LOG_GENERIC) };
    const Logger logInit{ LOG_INIT,    GetDefaultLevelForLogger(LOG_INIT) };
    const Logger logConfig{ LOG_CONFIG,  GetDefaultLevelForLogger(LOG_CONFIG) };
    const Logger logGraphic{ LOG_GRAPHIC, GetDefaultLevelForLogger(LOG_GRAPHIC) };

} // namespace Interstellar::Logging
