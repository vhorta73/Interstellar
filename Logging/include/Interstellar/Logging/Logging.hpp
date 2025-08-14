#pragma once

#include <spdlog/spdlog.h>
#include <fmt/format.h>  // needed for fmt::format_string when SPDLOG_FMT_EXTERNAL=ON
#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include "Interstellar/Logging/LogLevel.hpp"

namespace Interstellar::Logging {

    /**
     * @def LOG_GENERIC
     * @brief Default logger name for general-purpose logging.
     */
    inline constexpr std::string_view LOG_GENERIC = "Interstellar";

    /**
     * @def LOG_INIT
     * @brief Logger name for system or engine initialization events.
     */
    inline constexpr std::string_view LOG_INIT = "Initialisation";

    /**
     * @def LOG_CONFIG
     * @brief Logger name for configuration-related output.
     */
    inline constexpr std::string_view LOG_CONFIG = "Config";

    /**
     * @def LOG_GRAPHIC
     * @brief Logger name for graphics and rendering system messages.
     */
    inline constexpr std::string_view LOG_GRAPHIC = "Graphic";

    /**
     * @ingroup InterstellarCore Inberstellar Core
     * @class Logger
     * @brief A wrapper around spdlog for consistent, categorized logging across the engine.
     *
     * Supports log levels, formatted output, and adaptive flushing based on message frequency.
     *
     * @since 1.0
     */
    class Logger {
    public:
        /// Default constructor - creates a logger with default name and level.
        Logger();

        /// Destructor ensures pending log data is flushed.
        ~Logger() noexcept { if (m_Logger) m_Logger->flush(); }

        /// Construct with logger name and log level.
        explicit Logger(std::string_view loggerName, LogLevel level);

        /// Construct with logger name (uses default level).
        explicit Logger(std::string_view loggerName);

        /// Construct with level only (uses default logger name).
        explicit Logger(LogLevel level);

        // === Unformatted (basic) logging ===
        void LogCritical(const std::string& msg) const;
        void LogError(const std::string& msg) const;
        void LogWarn(const std::string& msg) const;
        void LogDebug(const std::string& msg) const;
        void LogInfo(const std::string& msg) const;
        void LogTrace(const std::string& msg) const;

        // === Formatted logging ===

        /**
         * @brief Log a critical-level message with formatting.
         * @tparam Args Format arguments.
         * @param fmt Format string.
         */
        template <typename... Args>
        void LogCritical(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->critical(fmt, std::forward<Args>(args)...);
            //m_Logger->flush();
        }

        /**
         * @brief Log an error-level message with formatting. May trigger a flush.
         * @tparam Args Format arguments.
         * @param fmt Format string.
         */
        template <typename... Args>
        void LogError(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->error(fmt, std::forward<Args>(args)...);
        }

        /**
         * @brief Log a warning-level message.
         */
        template <typename... Args>
        void LogWarn(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->warn(fmt, std::forward<Args>(args)...);
        }

        /**
         * @brief Log a debug-level message.
         */
        template <typename... Args>
        void LogDebug(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->debug(fmt, std::forward<Args>(args)...);
        }

        /**
         * @brief Log an info-level message.
         */
        template <typename... Args>
        void LogInfo(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->info(fmt, std::forward<Args>(args)...);
        }

        /**
         * @brief Log a trace-level message.
         */
        template <typename... Args>
        void LogTrace(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->trace(fmt, std::forward<Args>(args)...);
        }

        /**
         * @brief Access the underlying spdlog instance.
         * @return Shared pointer to the spdlog logger.
         */
        [[nodiscard]] std::shared_ptr<spdlog::logger> GetSpdLogger() const noexcept { return m_Logger; }

    private:
        std::shared_ptr<spdlog::logger> m_Logger;

    };

    // One global instance per well-known logger name (declared here) ---
    extern const Logger logGeneric;   // "Interstellar"
    extern const Logger logInit;      // "Initialisation"
    extern const Logger logConfig;    // "Config"
    extern const Logger logGraphic;   // "Graphic"

    // Optional: convenience aliases if you prefer shorter names in call-sites
    inline const Logger& LogGeneric() { return logGeneric; }
    inline const Logger& LogInit() { return logInit; }
    inline const Logger& LogConfig() { return logConfig; }
    inline const Logger& LogGraphic() { return logGraphic; }
}
