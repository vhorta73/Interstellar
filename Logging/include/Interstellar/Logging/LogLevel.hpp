#pragma once
/**
 * @file
 * @brief Severity levels and helpers for the Interstellar logging API.
 * @since 1.0
 */

#include <string_view>
#include <optional>
#include <ostream>
#include <stdexcept> // std::invalid_argument

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

    // --- internal: ASCII case-insensitive compare (no locale) ----------------
    namespace detail {
        // Keep these constexpr; they're fine on MSVC. The public parsers below are inline (non-constexpr).
        constexpr char ascii_tolower(char c) noexcept {
            return (c >= 'A' && c <= 'Z') ? static_cast<char>(c | 0x20) : c;
        }
        constexpr bool iequals(std::string_view a, std::string_view b) noexcept {
            if (a.size() != b.size()) return false;
            for (size_t i = 0; i < a.size(); ++i) {
                if (ascii_tolower(a[i]) != ascii_tolower(b[i])) return false;
            }
            return true;
        }
    } // namespace detail

    /**
     * @brief ASCII case-insensitive parse to LogLevel.
     * @details
     * Accepted values:
     * - long names: "trace","debug","info","warn","warning","error","critical","fatal"
     * - short aliases: **"err"** -> Error, **"crit"** -> Critical
     *
     * @return parsed level or std::nullopt if not recognized.
     * @ingroup logging_api
     * @since 1.0
     */
    constexpr std::optional<LogLevel> TryParseLogLevel(std::string_view s) noexcept {
        using detail::iequals;
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
    * @brief Back-compat: parse a level name into a LogLevel.
    * @return The parsed level, or std::nullopt if not recognized.
    * @note ASCII-only, does not trim whitespace.
    * @ingroup logging_api
    * 
    * @since 1.0
    */
    constexpr std::optional<LogLevel> FromString(std::string_view s) noexcept {
        return TryParseLogLevel(s);
    }

    /**
    * @brief Parse to LogLevel or return a fallback if unrecognized.
    * @ingroup logging_api
    * @since 1.0
    */
    constexpr LogLevel ParseLogLevelOr(std::string_view s, LogLevel fallback) noexcept {
        if (auto v = TryParseLogLevel(s)) return *v;
        return fallback;
    }

    /**
     * @brief Parse to LogLevel or throw std::invalid_argument if unrecognized.
     * @ingroup logging_api
     * @since 1.0
     */
    inline LogLevel ParseLogLevel(std::string_view s) {
        if (auto v = TryParseLogLevel(s)) return *v;
        throw std::invalid_argument("Unknown log level: '" + std::string(s) + "'");
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
