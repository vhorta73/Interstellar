#pragma once
/**
 * @file
 * @ingroup Logging
 * @brief Severity levels and helpers for the Interstellar logging API.
 * @since 1.0
 */

#include <string_view>
#include <optional>
#include <ostream>
#include <stdexcept> // std::invalid_argument

namespace Interstellar::Logging {

    /** 
     *  \ingroup Logging
     *  \brief Public logging facilities for the Interstellar codebase.
     *  \since 1.0
     */

     /**
      * @enum LogLevel
      * @ingroup Logging
      * @brief Logging severity in ascending order of importance.
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
     * @ingroup Logging
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
        return "INFO";
    }

    // internal ASCII helpers
    namespace detail {
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
     * @details Accepts: "trace","debug","info","warn"/"warning","error"/"err","critical"/"fatal"/"crit".
     * @return parsed level or std::nullopt if not recognized.
     * @ingroup Logging
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
     * @brief Back-compat: alias to TryParseLogLevel.
     * @return parsed level or std::nullopt if not recognized.
     * @ingroup Logging
     * @since 1.0
     */
    constexpr std::optional<LogLevel> FromString(std::string_view s) noexcept {
        return TryParseLogLevel(s);
    }

    /**
     * @brief Parse to LogLevel or return a fallback if unrecognized.
     * @ingroup Logging
     * @since 1.0
     */
    constexpr LogLevel ParseLogLevelOr(std::string_view s, LogLevel fallback) noexcept {
        if (auto v = TryParseLogLevel(s)) return *v;
        return fallback;
    }

    /**
     * @brief Parse to LogLevel or throw std::invalid_argument if unrecognized.
     * @ingroup Logging
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
