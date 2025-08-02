#pragma once

#include <spdlog/spdlog.h>
#include "Interstellar/Core/LogLevel.hpp"

namespace Interstellar::Core {

    inline constexpr const char* LOG_GENERIC = "Interstellar";
    inline constexpr const char* LOG_INIT    = "Initialisation";
    inline constexpr const char* LOG_CONFIG  = "Config";
    inline constexpr const char* LOG_GRAPHIC = "Graphic";

    /**
    * @class Logger
    * @brief A lightweight wrapper around spdlog for unified engine logging.
    *
    * Provides structured, categorized logging with optional format support.
    * Defaults to console output but can be extended to other sinks.
    *
    * @since 1.0
    */
    class Logger {
    public:
        Logger();
        ~Logger() {
            if (m_Logger) {
                m_Logger->flush();
            }
        }

        explicit Logger(const std::string& loggerName, LogLevel level);
        explicit Logger(const std::string& loggerName);
        explicit Logger(LogLevel level);

        // ==== Basic log methods (non-formatted) ====
        void LogCritical(const std::string& msg) const;
        void LogError(const std::string& msg) const;
        void LogWarn(const std::string& msg) const;
        void LogDebug(const std::string& msg) const;
        void LogInfo(const std::string& msg) const;
        void LogTrace(const std::string& msg) const;

        // ==== Formatted log method (templated) ====
        template <typename... Args>
        void LogCritical(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->critical(fmt, std::forward<Args>(args)...);
            m_Logger->flush();
        }

        template <typename... Args>
        void LogError(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->error(fmt, std::forward<Args>(args)...);

            if (shouldFlushAdaptive()) {
                m_Logger->flush();
            }
        }

        bool shouldFlushAdaptive() const {
            auto now = std::chrono::steady_clock::now();
            auto msSinceLast = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFlushTime).count();

            if (msSinceLast > 1000) {
                flushCounter = 0; // Reset after 1 sec of quiet
                lastFlushTime = now;
            }
            else {
              flushCounter++;
            }

            // Flush on 1st, 2nd, 4th, 8th error, etc.
            return (flushCounter & (flushCounter - 1)) == 0;
        }

        template <typename... Args>
        void LogWarn(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->warn(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void LogDebug(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->debug(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void LogInfo(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->info(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void LogTrace(fmt::format_string<Args...> fmt, Args&&... args) const {
            m_Logger->trace(fmt, std::forward<Args>(args)...);
        }

        /**
        * @brief Access to the internal spdlog logger instance.
        * @return A shared pointer to the underlying spdlog logger.
        */
        std::shared_ptr<spdlog::logger> GetSpdLogger() const { return m_Logger; }

    private:
        mutable int flushCounter = 0; // Counter for adaptive flushing
        mutable std::chrono::steady_clock::time_point lastFlushTime = std::chrono::steady_clock::now();
        std::shared_ptr<spdlog::logger> m_Logger;
    };
}
