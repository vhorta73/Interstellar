#pragma once

#include <spdlog/spdlog.h>
#include <chrono>
#include <memory>
#include <string>
#include "Interstellar/Core/LogLevel.hpp"

namespace Interstellar::Core {

    /**
     * @def LOG_GENERIC
     * @brief Default logger name for general-purpose logging.
     */
    inline constexpr const char* LOG_GENERIC = "Interstellar";

    /**
     * @def LOG_INIT
     * @brief Logger name for system or engine initialization events.
     */
    inline constexpr const char* LOG_INIT = "Initialisation";

    /**
     * @def LOG_CONFIG
     * @brief Logger name for configuration-related output.
     */
    inline constexpr const char* LOG_CONFIG = "Config";

    /**
     * @def LOG_GRAPHIC
     * @brief Logger name for graphics and rendering system messages.
     */
    inline constexpr const char* LOG_GRAPHIC = "Graphic";

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
        /// Default constructor — creates a logger with default name and level.
        Logger();

        /// Destructor ensures pending log data is flushed.
        ~Logger() {
            if (m_Logger) {
                m_Logger->flush();
            }
        }

        /// Construct with logger name and log level.
        explicit Logger(const std::string& loggerName, LogLevel level);

        /// Construct with logger name (uses default level).
        explicit Logger(const std::string& loggerName);

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
            m_Logger->flush();
        }

        /**
         * @brief Log an error-level message with formatting. May trigger a flush.
         * @tparam Args Format arguments.
         * @param fmt Format string.
         */
        template <typename... Args>
        void LogError(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->error(fmt, std::forward<Args>(args)...);
            if (shouldFlushAdaptive()) {
                m_Logger->flush();
            }
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
        [[nodiscard]] std::shared_ptr<spdlog::logger> GetSpdLogger() const { return m_Logger; }

    private:
        mutable int flushCounter = 0; ///< Adaptive flush counter
        mutable std::chrono::steady_clock::time_point lastFlushTime = std::chrono::steady_clock::now();

        std::shared_ptr<spdlog::logger> m_Logger;

        /**
         * @brief Heuristic flush control for error logging.
         *
         * This method implements an adaptive strategy to reduce excessive flushing
         * while still ensuring frequent errors don't get delayed indefinitely.
         *
         * Flushes on the 1st, 2nd, 4th, 8th, etc. error if within 1 second.
         *
         * @return true if the logger should flush, false otherwise.
         */
        [[nodiscard]] bool shouldFlushAdaptive() const {
            auto now = std::chrono::steady_clock::now();
            auto msSinceLast = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFlushTime).count();

            if (msSinceLast > 1000) {
                flushCounter = 0;
                lastFlushTime = now;
            }
            else {
                ++flushCounter;
            }

            // Flush on powers of two (1, 2, 4, 8, ...)
            return (flushCounter & (flushCounter - 1)) == 0;
        }
    };

} // namespace Interstellar::Core
