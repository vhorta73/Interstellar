#pragma once

#include <string>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>

#include "Interstellar/Logging/LogLevel.hpp"

// Add this include for fmt
#include <fmt/format.h>

// Add this include for glm::vec3
#include <glm/vec3.hpp>

/**
 * @file ParseLogLevel.hpp
 * @defgroup LogLevelUtils Log Level Utilities
 * @ingroup InterstellarCore
 *
 * @brief Provides string parsing for log levels.
 *
 * @since 1.0
 */

namespace Interstellar::Logging {

    /**
     * @brief Converts a string (case-insensitive) into a corresponding LogLevel enum.
     *
     * Accepts values such as: `"trace"`, `"debug"`, `"info"`, `"warn"`, `"error"`, `"critical"` (case-insensitive).
     * Throws a runtime error if the input does not match any known log level.
     *
     * @param s Input string representing the log level
     * @return LogLevel corresponding to the string
     *
     * @throws std::runtime_error if the string is not a recognized log level
     *
     * @code
     * LogLevel level = ParseLogLevel("DEBUG"); // returns LogLevel::Debug
     * @endcode
     *
     * @since 1.0
     */
    inline LogLevel ParseLogLevel(const std::string& s) {
        static const std::unordered_map<std::string, LogLevel> lookup = {
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