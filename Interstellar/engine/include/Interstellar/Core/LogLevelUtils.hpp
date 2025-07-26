#pragma once

#include <string>
#include <map>
#include <algorithm>
#include <stdexcept>

#include "Interstellar/Core/LogLevel.hpp"

namespace Interstellar::Core {

    inline LogLevel ParseLogLevel(const std::string& s) {
        static const std::map<std::string, LogLevel> lookup = {
            {"trace",    LogLevel::Trace},
            {"debug",    LogLevel::Debug},
            {"info",     LogLevel::Info},
            {"warn",     LogLevel::Warn},
            {"error",    LogLevel::Error},
            {"critical", LogLevel::Critical}
        };

        std::string lower = s;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        auto it = lookup.find(lower);
        if (it == lookup.end())
            throw std::runtime_error("Unknown log level: " + s);

        return it->second;
    }
}
