#pragma once

#include <spdlog/spdlog.h>

namespace Interstellar::Core {

    inline constexpr const char* GENERIC_LOG = "Interstellar";
    inline constexpr const char* CONFIG_LOG  = "Config";
    inline constexpr const char* GRAPHIC_LOG = "Graphic";

    // Logger class defaulting to GENERIC_LOG.
    class Logger {
    public:
        Logger();
        Logger(const std::string& loggerName);

        void LogInfo(const std::string& msg);
        void LogWarn(const std::string& msg);
        void LogError(const std::string& msg);
        void LogDebug(const std::string& msg);
        void LogCritical(const std::string& msg);

    private:
        std::shared_ptr<spdlog::logger> m_Logger;
    };
}