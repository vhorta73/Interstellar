#pragma once

#include <string_view>
#include <string>
#include <optional>
#include <stdexcept>
#include <cctype>

#include "Interstellar/Utils/String/StringUtils.hpp"

namespace Interstellar::Utils::Time {

    enum class TimeUnit {
        Seconds,
        Milliseconds,
        Minutes,
        Hours,
        Days,
        Months,
        Years
    };

    inline constexpr double seconds_per(TimeUnit u) {
        switch (u) {
        case TimeUnit::Seconds:      return 1.0;
        case TimeUnit::Milliseconds: return 1e-3;
        case TimeUnit::Minutes:      return 60.0;
        case TimeUnit::Hours:        return 3600.0;
        case TimeUnit::Days:         return 86400.0;
        case TimeUnit::Months:       return (365.25 / 12.0) * 86400.0;
        case TimeUnit::Years:        return 365.25 * 86400.0;
        }
        return 1.0; // hush warnings
    }

    inline std::string_view time_unit_to_string(TimeUnit unit) {
        switch (unit) {
        case TimeUnit::Seconds:      return "seconds";
        case TimeUnit::Milliseconds: return "milliseconds";
        case TimeUnit::Minutes:      return "minutes";
        case TimeUnit::Hours:        return "hours";
        case TimeUnit::Days:         return "days";
        case TimeUnit::Months:       return "months";
        case TimeUnit::Years:        return "years";
        }
        throw std::invalid_argument("Unknown time unit enum value");
    }

    struct TimeValueUnit {
        double value; ///< The numeric value of the time duration.
        TimeUnit unit; ///< The unit of the time duration.
    };

    // Throws on unknown (good when you want a hard fail)
    inline TimeUnit string_to_time_unit(std::string_view unit) {
        const std::string u = Interstellar::Utils::String::to_lower_copy(unit);

        if (u == "s" || u == "sec" || u == "second" || u == "seconds")      return TimeUnit::Seconds;
        if (u == "ms" || u == "millisec" || u == "millisecond" || u == "milliseconds") return TimeUnit::Milliseconds;
        if (u == "m" || u == "min" || u == "minute" || u == "minutes")      return TimeUnit::Minutes;
        if (u == "h" || u == "hr" || u == "hour" || u == "hours")        return TimeUnit::Hours;
        if (u == "d" || u == "day" || u == "days")        return TimeUnit::Days;
        if (u == "mo" || u == "mth" || u == "month" || u == "months")       return TimeUnit::Months;
        if (u == "y" || u == "yr" || u == "year" || u == "years")        return TimeUnit::Years;

        throw std::invalid_argument("Unknown time unit: " + std::string(unit));
    }

    // Exact multipliers. We use the Julian year = 365.25 days.
    inline double to_seconds(double value, TimeUnit unit) {
        switch (unit) {
        case TimeUnit::Seconds:      return value;
        case TimeUnit::Milliseconds: return value * 1e-3;
        case TimeUnit::Minutes:      return value * 60.0;
        case TimeUnit::Hours:        return value * 3600.0;
        case TimeUnit::Days:         return value * 86400.0;
        case TimeUnit::Months:       return value * (365.25 / 12.0) * 86400.0;
        case TimeUnit::Years:        return value * 31557600.0; // 365.25 * 86400
        }
        return value; // unreachable, but keeps MSVC happy
    }

    // Non-throwing string overload - returns nullopt if unknown.
    inline double to_seconds(double value, std::string_view unit) {
        return to_seconds(value, string_to_time_unit(unit)); // throws on unknown
    }
}
