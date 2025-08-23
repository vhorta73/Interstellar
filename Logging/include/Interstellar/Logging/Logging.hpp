#pragma once
/**
 * @file
 * @ingroup Logging
 * @brief Interstellar logging facade built on top of spdlog.
 *
 * @details
 * Public, category-based logging API used across the Interstellar codebase.
 * This header documents the public contract:
 * - Thread-safety: logging calls are thread-safe (multithreaded sinks).
 * - Flush semantics: errors (and above) flush promptly; a periodic background
 *   flush is enabled; destructors flush as a last safeguard.
 * - Log files: by default, files go under "./logs" (relative to the process
 *   working directory). Set INTERSTELLAR_LOG_DIR to override the directory.
 *
 * @since 1.0
 */

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "Interstellar/Logging/LogLevel.hpp"

namespace Interstellar::Logging {

     /**
      * @name Well-known logger names
      * @ingroup Logging
      * @brief Category names used by the global logger instances.
      * @since 1.0
      * @{
      */
    inline constexpr std::string_view LOG_GENERIC = "Interstellar";   ///< @since 1.0
    inline constexpr std::string_view LOG_INIT = "Initialisation"; ///< @since 1.0
    inline constexpr std::string_view LOG_CONFIG = "Config";         ///< @since 1.0
    inline constexpr std::string_view LOG_GRAPHIC = "Graphic";        ///< @since 1.0
    /** @} */

    /**
     * @class Logger
     * @ingroup Logging
     * @brief Thin facade over spdlog providing a consistent, category-based API.
     *
     * @par Flush semantics
     * - Error and higher severities flush promptly.
     * - A periodic background flush is enabled to keep buffers moving.
     * - The destructor flushes the underlying logger as a final safeguard.
     *
     * @par Files and environment
     * - Log files are written under "./logs" by default.
     * - Set INTERSTELLAR_LOG_DIR (absolute or relative) to change the directory.
     *
     * @par Example
     * @code
     * using namespace Interstellar::Logging;
     * logConfig.LogInfo("Loaded {} entries", count);
     * Logger custom("AI", LogLevel::Warn);
     * custom.LogWarn("Pathfinding took {} ms", dt_ms);
     * @endcode
     *
     * @since 1.0
     */
    class Logger {
    public:
        /// Create a logger bound to LOG_GENERIC with its default level. @since 1.0
        Logger();

        /// Destructor flushes any pending data (no-throw). @since 1.0
        ~Logger() noexcept;

        /// Construct a logger for a specific loggerName and level. @since 1.0
        explicit Logger(std::string_view loggerName, LogLevel level);
        /// Construct a logger for loggerName using that category's default level. @since 1.0
        explicit Logger(std::string_view loggerName);
        /// Construct a logger with level bound to LOG_GENERIC. @since 1.0
        explicit Logger(LogLevel level);

        // -------- Unformatted (basic) logging --------
        void LogCritical(const std::string& msg) const; ///< @since 1.0
        void LogError(const std::string& msg) const;    ///< @since 1.0
        void LogWarn(const std::string& msg) const;     ///< @since 1.0
        void LogDebug(const std::string& msg) const;    ///< @since 1.0
        void LogInfo(const std::string& msg) const;     ///< @since 1.0
        void LogTrace(const std::string& msg) const;    ///< @since 1.0

        // -------- Formatted logging (fmt-safe) --------
        template <typename... Args> void LogCritical(fmt::format_string<Args...> f, Args&&... a) const { m_Logger->critical(f, std::forward<Args>(a)...); } ///< @since 1.0
        template <typename... Args> void LogError(fmt::format_string<Args...> f, Args&&... a) const { m_Logger->error(f, std::forward<Args>(a)...); } ///< @since 1.0
        template <typename... Args> void LogWarn(fmt::format_string<Args...> f, Args&&... a) const { m_Logger->warn(f, std::forward<Args>(a)...); } ///< @since 1.0
        template <typename... Args> void LogDebug(fmt::format_string<Args...> f, Args&&... a) const { m_Logger->debug(f, std::forward<Args>(a)...); } ///< @since 1.0
        template <typename... Args> void LogInfo(fmt::format_string<Args...> f, Args&&... a) const { m_Logger->info(f, std::forward<Args>(a)...); } ///< @since 1.0
        template <typename... Args> void LogTrace(fmt::format_string<Args...> f, Args&&... a) const { m_Logger->trace(f, std::forward<Args>(a)...); } ///< @since 1.0

        /// Access the underlying spdlog logger. @since 1.0
        [[nodiscard]] std::shared_ptr<spdlog::logger> GetSpdLogger() const noexcept { return m_Logger; }

    private:
        std::shared_ptr<spdlog::logger> m_Logger; ///< Owned spdlog logger instance.
    };

    /**
     * @name Global category loggers
     * @ingroup Logging
     * @brief One process-wide instance per well-known category.
     * @since 1.0
     * @{
     */
    extern const Logger logGeneric;  ///< Logger bound to LOG_GENERIC. @since 1.0
    extern const Logger logInit;     ///< Logger bound to LOG_INIT.    @since 1.0
    extern const Logger logConfig;   ///< Logger bound to LOG_CONFIG.  @since 1.0
    extern const Logger logGraphic;  ///< Logger bound to LOG_GRAPHIC. @since 1.0
    /** @} */

    /**
     * @name Convenience accessors
     * @ingroup Logging
     * @since 1.0
     * @{
     */
    inline const Logger& LogGeneric() { return logGeneric; } ///< @since 1.0
    inline const Logger& LogInit() { return logInit; } ///< @since 1.0
    inline const Logger& LogConfig() { return logConfig; } ///< @since 1.0
    inline const Logger& LogGraphic() { return logGraphic; } ///< @since 1.0
    /** @} */

} // namespace Interstellar::Logging
