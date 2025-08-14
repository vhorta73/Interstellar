#pragma once
/**
 * @file
 * @brief Severity levels used by the Interstellar logging API.
 */

#include <string_view>
#include <optional>
#include <ostream>

namespace Interstellar::Logging {

    /**
     * @enum LogLevel
     * @brief Logging severity in ascending order of importance.
     *
     * Lower levels (Trace/Debug) are noisy and often disabled in production,
     * higher levels (Error/Critical) should be rare and demand attention.
     */
    enum class LogLevel {
        Trace,    ///< Finest-grain diagnostics; very verbose.
        Debug,    ///< Developer-oriented details useful while debugging.
        Info,     ///< High-level runtime information about normal operation.
        Warn,     ///< Something unexpected happened, but the program can continue.
        Error,    ///< An operation failed; the program continues in a degraded path.
        Critical  ///< Severe problem requiring immediate attention; likely unrecoverable.
    };

    /**
     * @brief Convert a LogLevel to a human-readable name.
     * @return Uppercase name (TRACE, DEBUG, ...).
     */
    constexpr std::string_view ToString(LogLevel lvl) noexcept {
        switch (lvl) {
        case LogLevel::Trace:    return "TRACE";
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warn:     return "WARN";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        }
        return "INFO";
    }

    /**
     * @brief Parse a level name (case-insensitive) into a LogLevel.
     * @param s Accepted: "trace","debug","info","warn","warning","error","critical","fatal".
     * @return The parsed level or std::nullopt if not recognized.
     */
    inline std::optional<LogLevel> FromString(std::string_view s) noexcept {
        auto ieq = [](char a, char b) { return (a | 0x20) == (b | 0x20); }; // ASCII lowercase compare
        auto iequals = [&](std::string_view a, std::string_view b) {
            if (a.size() != b.size()) return false;
            for (size_t i = 0; i < a.size(); ++i) if (!ieq(a[i], b[i])) return false;
            return true;
            };

        if (iequals(s, "trace"))    return LogLevel::Trace;
        if (iequals(s, "debug"))    return LogLevel::Debug;
        if (iequals(s, "info"))     return LogLevel::Info;
        if (iequals(s, "warn") || iequals(s, "warning")) return LogLevel::Warn;
        if (iequals(s, "error"))    return LogLevel::Error;
        if (iequals(s, "critical") || iequals(s, "fatal")) return LogLevel::Critical;
        return std::nullopt;
    }

    /**
     * @brief Stream a level as its uppercase name.
     */
    inline std::ostream& operator<<(std::ostream& os, LogLevel lvl) {
        return os << ToString(lvl);
    }

} // namespace Interstellar::Logging
