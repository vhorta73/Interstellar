#pragma once

#include <spdlog/spdlog.h>

namespace Interstellar::Core {

    inline constexpr const char* GENERIC_LOG = "Interstellar";
    inline constexpr const char* CONFIG_LOG  = "Config";
    inline constexpr const char* GRAPHIC_LOG = "Graphic";

    class Logger {
    public:
        Logger();
        explicit Logger(const std::string& loggerName);

        void LogInfo(const std::string& msg);
        void LogWarn(const std::string& msg);
        void LogError(const std::string& msg);
        void LogDebug(const std::string& msg);
        void LogCritical(const std::string& msg);

        template <typename... Args>
        void LogInfo(fmt::format_string<Args...> fmt, Args&&... args) {
            m_Logger->info(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void LogWarn(fmt::format_string<Args...> fmt, Args&&... args) {
            m_Logger->warn(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void LogError(fmt::format_string<Args...> fmt, Args&&... args) {
            m_Logger->error(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void LogDebug(fmt::format_string<Args...> fmt, Args&&... args) {
            m_Logger->debug(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void LogCritical(fmt::format_string<Args...> fmt, Args&&... args) {
            m_Logger->critical(fmt, std::forward<Args>(args)...);
        }

    private:
        std::shared_ptr<spdlog::logger> m_Logger;
    };
}
