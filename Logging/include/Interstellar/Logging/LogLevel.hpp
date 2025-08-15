#pragma once
/**
 * @file
 * @brief Severity levels and helpers for the Interstellar logging API.
 * @since 1.0
 */

#include <string_view>
#include <optional>
#include <ostream>

namespace Interstellar::Logging {

    /**
     * @enum LogLevel
     * @ingroup logging_api
     * @brief Logging severity in ascending order of importance.
     *
     * Lower levels (Trace/Debug) are noisy and often disabled in production;
     * higher levels (Error/Critical) should be rare and demand attention.
     *
     * @since 1.0
     */
    enum class LogLevel {
        Trace,    ///< Finest-grain diagnostics; very verbose. @since 1.0
        Debug,    ///< Developer-oriented details useful while debugging. @since 1.0
        Info,     ///< High-level runtime information about normal operation. @since 1.0
        Warn,     ///< Something unexpected happened, but the program can continue. @since 1.0
        Error,    ///< An operation failed; the program continues in a degraded path. @since 1.0
        Critical  ///< Severe problem requiring immediate attention; likely unrecoverable. @since 1.0
    };

    /**
     * @brief Convert a LogLevel to a human-readable uppercase name.
     * @return One of: TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL.
     * @ingroup logging_api
     * @since 1.0
     */
    [[nodiscard]] constexpr std::string_view ToString(LogLevel lvl) noexcept {
        switch (lvl) {
        case LogLevel::Trace:    return "TRACE";
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warn:     return "WARN";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        }
        // Defensive fallback (should be unreachable with a closed enum).
        return "INFO";
    }

    /**
     * @brief Parse a level name (ASCII case-insensitive) into a LogLevel.
     * @param s Accepted: "trace","debug","info","warn","warning","error","critical","fatal"
     *           and short aliases "err","crit".
     * @return The parsed level, or std::nullopt if not recognized.
     * @note Parsing is ASCII only (not locale aware) and does not trim whitespace.
     * @ingroup logging_api
     * @since 1.0
     */
    [[nodiscard]] constexpr std::optional<LogLevel> FromString(std::string_view s) noexcept {
        auto ieq = [](char a, char b) { return (a | 0x20) == (b | 0x20); }; // ASCII case-fold compare
        auto iequals = [&](std::string_view a, std::string_view b) {
            if (a.size() != b.size()) return false;
            for (size_t i = 0; i < a.size(); ++i) {
                if (!ieq(a[i], b[i])) return false;
            }
            return true;
            };

        if (iequals(s, "trace"))                         return LogLevel::Trace;
        if (iequals(s, "debug"))                         return LogLevel::Debug;
        if (iequals(s, "info"))                          return LogLevel::Info;
        if (iequals(s, "warn") || iequals(s, "warning")) return LogLevel::Warn;
        if (iequals(s, "error") || iequals(s, "err"))    return LogLevel::Error;
        if (iequals(s, "critical") || iequals(s, "fatal") || iequals(s, "crit"))
            return LogLevel::Critical;

        return std::nullopt;
    }

    /**
     * @brief Stream a level as its uppercase name.
     * @ingroup logging_api
     * @since 1.0
     */
    inline std::ostream& operator<<(std::ostream& os, LogLevel lvl) {
        return os << ToString(lvl);
    }

} // namespace Interstellar::Logging
