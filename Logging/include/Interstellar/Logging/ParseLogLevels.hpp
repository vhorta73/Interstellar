#pragma once
/**
 * @file
 * @brief Helpers to parse log levels from strings.
 *
 * Provides a no-throw parser that returns std::optional, and a convenience
 * wrapper that throws on failure. Both delegate to Interstellar::Logging::FromString.
 *
 * @since 1.0
 */

#include <string_view>
#include <optional>
#include <stdexcept>

#include "Interstellar/Logging/LogLevel.hpp"

namespace Interstellar::Logging {

    /**
     * @brief Parse a level name (ASCII case-insensitive) without throwing.
     * @param s Input such as "trace","debug","info","warn","warning","error","critical","fatal"
     *          (and short aliases "err","crit" if enabled in @ref FromString).
     * @return Parsed level or std::nullopt if not recognized.
     * @ingroup logging_api
     * @since 1.0
     */
    [[nodiscard]] constexpr std::optional<LogLevel> TryParseLogLevel(std::string_view s) noexcept {
        return FromString(s);
    }

    /**
     * @brief Parse a level name or return a fallback.
     * @param s   Input string.
     * @param def Fallback level if parsing fails.
     * @return Parsed level or @p def.
     * @ingroup logging_api
     * @since 1.0
     */
    [[nodiscard]] constexpr LogLevel ParseLogLevelOr(std::string_view s, LogLevel def) noexcept {
        if (auto lvl = FromString(s)) return *lvl;
        return def;
    }

    /**
     * @brief Parse a level name, throwing on failure.
     * @param s Input string.
     * @return Parsed LogLevel.
     * @throws std::invalid_argument if @p s is not a recognized level.
     * @ingroup logging_api
     * @since 1.0
     */
    [[nodiscard]] inline LogLevel ParseLogLevel(std::string_view s) {
        if (auto lvl = FromString(s)) return *lvl;
        throw std::invalid_argument("Unknown log level: " + std::string{ s });
    }

} // namespace Interstellar::Logging
