#pragma once
/**
 * @file
 * @brief Interstellar logging facade built on top of spdlog.
 *
 * This header declares the @ref Interstellar::Logging::Logger class and a set of
 * named global logger instances for common categories (generic, init, config, graphics).
 *
 * @details
 * - Thread-safety: spdlog is thread-safe with the multithreaded sinks used here.
 * - Global policy (flush pattern, periodic flushing) is initialized once per process,
 *   in the implementation file.
 * - Log files are written under the process working directory by default (./logs).
 *   You can override the directory with the environment variable
 *   `INTERSTELLAR_LOG_DIR` (absolute or relative path).
 */

#include <spdlog/spdlog.h>
#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include "Interstellar/Logging/LogLevel.hpp"

namespace Interstellar::Logging {

    /**
     * @defgroup logging_api Logging API
     * @brief Public logging facilities for the Interstellar codebase.
     * @{
     */

     /**
      * @name Well-known logger names
      * @brief Category names used by the global logger instances.
      * @{
      */

      /// Default logger name for general-purpose logging.
    inline constexpr std::string_view LOG_GENERIC = "Interstellar";

    /// Logger name for system or engine initialization events.
    inline constexpr std::string_view LOG_INIT = "Initialisation";

    /// Logger name for configuration-related messages.
    inline constexpr std::string_view LOG_CONFIG = "Config";

    /// Logger name for graphics / rendering messages.
    inline constexpr std::string_view LOG_GRAPHIC = "Graphic";

    /** @} */ // end of logger names

    /**
     * @class Logger
     * @brief Thin facade around spdlog to provide a consistent, category-based API.
     *
     * @par Flush semantics
     * - Errors `LogError()` and above will be flushed promptly due to the global policy
     *   set in the implementation (`flush_on(err)` and periodic `flush_every(2s)`).
     * - The destructor also flushes the underlying logger instance as a final safeguard.
     *
     * @par Performance
     * - Logging calls are cheap when disabled by level.
     * - For hot paths, prefer `LogDebug/LogTrace` and set levels accordingly.
     *
     * @par Environment
     * - `INTERSTELLAR_LOG_DIR` - if set, log files are written to this directory instead of `./logs`.
     *
     * @see LOG_GENERIC, LOG_INIT, LOG_CONFIG, LOG_GRAPHIC
     */
    class Logger {
    public:
        /// Create a logger bound to @ref LOG_GENERIC with its default level.
        Logger();

        /// Destructor flushes any pending data (no-throw).
        ~Logger() noexcept {
            if (m_Logger) m_Logger->flush();
        }

        /**
         * @brief Construct a logger for a specific @p loggerName and @p level.
         * @param loggerName Category name (e.g., @ref LOG_CONFIG).
         * @param level      Minimum level for this logger.
         */
        explicit Logger(std::string_view loggerName, LogLevel level);

        /**
         * @brief Construct a logger for a specific @p loggerName using that category's default level.
         * @param loggerName Category name (e.g., @ref LOG_GRAPHIC).
         */
        explicit Logger(std::string_view loggerName);

        /// Construct a logger with @p level bound to @ref LOG_GENERIC.
        explicit Logger(LogLevel level);

        // -------- Unformatted (basic) logging --------

        /// Log a critical-level message (always flushed by global policy).
        void LogCritical(const std::string& msg) const;

        /// Log an error-level message (flushed promptly by global policy).
        void LogError(const std::string& msg) const;

        /// Log a warning-level message.
        void LogWarn(const std::string& msg) const;

        /// Log a debug-level message.
        void LogDebug(const std::string& msg) const;

        /// Log an info-level message.
        void LogInfo(const std::string& msg) const;

        /// Log a trace-level message.
        void LogTrace(const std::string& msg) const;

        // -------- Formatted logging (fmt-safe) --------

        /**
         * @brief Log a critical-level message with formatting.
         * @tparam Args Parameter pack for format arguments.
         * @param fmt   Compile-time checked format string.
         * @param args  Arguments referenced by @p fmt.
         */
        template <typename... Args>
        void LogCritical(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->critical(fmt, std::forward<Args>(args)...);
        }

        /// Log an error-level formatted message.
        template <typename... Args>
        void LogError(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->error(fmt, std::forward<Args>(args)...);
        }

        /// Log a warning-level formatted message.
        template <typename... Args>
        void LogWarn(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->warn(fmt, std::forward<Args>(args)...);
        }

        /// Log a debug-level formatted message.
        template <typename... Args>
        void LogDebug(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->debug(fmt, std::forward<Args>(args)...);
        }

        /// Log an info-level formatted message.
        template <typename... Args>
        void LogInfo(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->info(fmt, std::forward<Args>(args)...);
        }

        /// Log a trace-level formatted message.
        template <typename... Args>
        void LogTrace(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->trace(fmt, std::forward<Args>(args)...);
        }

        /**
         * @brief Access the underlying spdlog logger.
         * @return Shared pointer to the spdlog logger.
         */
        [[nodiscard]] std::shared_ptr<spdlog::logger> GetSpdLogger() const noexcept { return m_Logger; }

    private:
        std::shared_ptr<spdlog::logger> m_Logger; ///< Owned spdlog logger instance.
    };

    /**
     * @name Global category loggers
     * @brief One process-wide instance per well-known category.
     * @details
     * These are defined in `Logging.cpp` and can be used directly:
     * @code
     *   using namespace Interstellar::Logging;
     *   logConfig.LogInfo("Loaded {} entries", count);
     *   logGraphic.LogError("Failed to compile shader: {}", reason);
     * @endcode
     * @{
     */
    extern const Logger logGeneric;  ///< Logger bound to @ref LOG_GENERIC.
    extern const Logger logInit;     ///< Logger bound to @ref LOG_INIT.
    extern const Logger logConfig;   ///< Logger bound to @ref LOG_CONFIG.
    extern const Logger logGraphic;  ///< Logger bound to @ref LOG_GRAPHIC.
    /** @} */ // end global loggers

    /**
     * @name Convenience accessors
     * @brief Header-only references if you prefer function syntax.
     * @{
     */
    inline const Logger& LogGeneric() { return logGeneric; }
    inline const Logger& LogInit() { return logInit; }
    inline const Logger& LogConfig() { return logConfig; }
    inline const Logger& LogGraphic() { return logGraphic; }
    /** @} */

    /** @} */ // end of group logging_api

} // namespace Interstellar::Logging
