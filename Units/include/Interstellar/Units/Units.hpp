#pragma once
/**
 * @file Units.hpp
 * @brief Single-file Interstellar Units facade (header-only, backend-agnostic API).
 *
 * Public API:
 *  - Types (PascalCase): Length, Time, Mass, Speed, Pressure, ...
 *  - Unit expressions:   Units::U::m, U::m2, U::m_per_s2, U::Pa, ...
 *  - Factories (lowercase domains):
 *        length::meters(v), length::square_meters(v), length::cubic_meters(v)
 *        time  ::seconds(v), time  ::minutes(v), time  ::hours(v)
 *        mass  ::kilograms(v), mass::grams(v), mass::tonnes(v)
 *        mech  ::pascals(v), mech::newtons(v), mech::joules(v), mech::watts(v),
 *                 mech::meters_per_second(v), mech::meters_per_second2(v)
 *        thermo::K_delta(v), thermo::K_absolute(v), thermo::as_deltaK(x)
 *  - Utilities:           in(q, U::unit), in_K(t), in_K_absolute(t_point)
 */

#include <mp-units/systems/si.h>
#include <mp-units/format.h>
#include <mp-units/math.h>

namespace Interstellar::Units {

    //========================
    // backend (hidden)
    //========================
    namespace detail {
        namespace mu = ::mp_units;
        namespace si = ::mp_units::si;
        namespace sym = ::mp_units::si::unit_symbols;
    }

    //========================
    // Types (public)
    //========================
    using Length = decltype(1.0 * detail::sym::m);
    using Area = decltype(1.0 * detail::sym::m2);
    using Volume = decltype(1.0 * detail::sym::m3);
    using Time = decltype(1.0 * detail::sym::s);
    using Mass = decltype(1.0 * detail::sym::kg);
    using Amount = decltype(1.0 * detail::sym::mol);

    using Speed = decltype(1.0 * detail::sym::m / detail::sym::s);
    using Acceleration = decltype(1.0 * detail::sym::m / (detail::sym::s * detail::sym::s));
    using Force = decltype(1.0 * detail::sym::N);
    using Pressure = decltype(1.0 * detail::sym::Pa);
    using Density = decltype(1.0 * detail::sym::kg / detail::sym::m3);
    using Energy = decltype(1.0 * detail::sym::J);
    using Power = decltype(1.0 * detail::sym::W);

    // Temperature types
    using TemperatureDeltaK = decltype(detail::mu::delta   <detail::sym::K>(1.0)); // quantity
    using TemperaturePointK = decltype(detail::mu::absolute<detail::sym::K>(0.0)); // quantity_point

    //========================
    // Shared unit catalog
    //========================
    namespace U {
        // base
        inline constexpr auto m = detail::sym::m;
        inline constexpr auto s = detail::sym::s;
        inline constexpr auto kg = detail::sym::kg;
        inline constexpr auto mol = detail::sym::mol;
        inline constexpr auto Pa = detail::sym::Pa;

        // derived
        inline constexpr auto m2 = detail::sym::m2;
        inline constexpr auto m3 = detail::sym::m3;
        inline constexpr auto m_per_s = detail::sym::m / detail::sym::s;
        inline constexpr auto m_per_s2 = detail::sym::m / (detail::sym::s * detail::sym::s);
        inline constexpr auto N = detail::sym::N;
        inline constexpr auto J = detail::sym::J;
        inline constexpr auto W = detail::sym::W;

        // time convenience
        inline constexpr auto minute = 60.0 * s;
        inline constexpr auto hour = 60.0 * minute;
        inline constexpr auto min = minute;
        inline constexpr auto hr = hour;

        inline constexpr auto W_per_m2 = W / m2;
    }

    //========================
    // Factories by domain
    //========================

    // Length / Area / Volume
    namespace length {
        inline auto meters(double v) -> Length { return v * U::m; }
        inline auto square_meters(double v) -> Area { return v * U::m2; }
        inline auto cubic_meters(double v) -> Volume { return v * U::m3; }

        inline constexpr auto km = 1000.0 * U::m;
        inline auto kilometers(double v) -> Length { return v * km; }
    }

    // Time
    namespace time {
        inline auto seconds(double v) -> Time { return v * U::s; }
        inline auto minutes(double v) -> Time { return v * U::minute; }
        inline auto hours(double v) -> Time { return v * U::hour; }
    }

    // Mass
    namespace mass {
        inline auto kilograms(double v) -> Mass { return v * U::kg; }
        inline auto grams(double v) -> Mass { return v * ((1.0 / 1000.0) * U::kg); } // <-- fixed
        inline auto tonnes(double v) -> Mass { return v * (1000.0 * U::kg); }
    }

    // Mechanics (speed, accel, force, energy, power, pressure)
    namespace mech {
        inline auto meters_per_second(double v) -> Speed { return v * U::m_per_s; }
        inline auto meters_per_second2(double v) -> Acceleration { return v * U::m_per_s2; }
        inline auto newtons(double v) -> Force { return v * U::N; }
        inline auto joules(double v) -> Energy { return v * U::J; }
        inline auto watts(double v) -> Power { return v * U::W; }
        inline auto pascals(double v) -> Pressure { return v * U::Pa; }
    }

    // Thermodynamics (Kelvin - offset safe)
    namespace thermo {
        inline auto K_delta(double v) -> TemperatureDeltaK { return detail::mu::delta   <detail::sym::K>(v); }
        inline auto K_absolute(double v) -> TemperaturePointK { return detail::mu::absolute<detail::sym::K>(v); }

        template<class T>
        constexpr auto as_deltaK(const T& t) {
            if constexpr (detail::mu::QuantityPoint<T>) return t - detail::si::absolute_zero;
            else                                        return t;
        }

        inline double in_K(const TemperatureDeltaK& t) {
            // Divide by a delta-K reference instead of (1.0 * K)
            return static_cast<double>(t / detail::mu::delta<detail::sym::K>(1.0));
        }

        inline double in_K_absolute(const TemperaturePointK& t_abs) {
            const auto dT = t_abs - detail::si::absolute_zero;
            return static_cast<double>(dT / detail::mu::delta<detail::sym::K>(1.0));
        }
    }

    //========================
    // Quantity-aware math (optional)
    //========================
    namespace qmath {
        template<class Q>
        inline auto sqrt(const Q& x) { return detail::mu::sqrt(x); }
    }

    //========================
    // User-defined literals (opt-in)
    //========================
    namespace literals {

        // Amount
        constexpr Amount operator"" _mol(long double v) { return static_cast<double>(v) * U::mol; }
        constexpr Amount operator"" _mol(unsigned long long v) { return static_cast<double>(v) * U::mol; }

        // Length
        constexpr Length operator"" _m(long double v) { return static_cast<double>(v) * U::m; }
        constexpr Length operator"" _m(unsigned long long v) { return static_cast<double>(v) * U::m; }

        // Area / Volume
        constexpr Area   operator"" _m2(long double v) { return static_cast<double>(v) * U::m2; }
        constexpr Area   operator"" _m2(unsigned long long v) { return static_cast<double>(v) * U::m2; }
        constexpr Volume operator"" _m3(long double v) { return static_cast<double>(v) * U::m3; }
        constexpr Volume operator"" _m3(unsigned long long v) { return static_cast<double>(v) * U::m3; }

        // Time
        constexpr Time operator"" _s(long double v) { return static_cast<double>(v) * U::s; }
        constexpr Time operator"" _s(unsigned long long v) { return static_cast<double>(v) * U::s; }

        // Mass
        constexpr Mass operator"" _kg(long double v) { return static_cast<double>(v) * U::kg; }
        constexpr Mass operator"" _kg(unsigned long long v) { return static_cast<double>(v) * U::kg; }

        // Pressure
        constexpr Pressure operator"" _Pa(long double v) { return static_cast<double>(v) * U::Pa; }
        constexpr Pressure operator"" _Pa(unsigned long long v) { return static_cast<double>(v) * U::Pa; }
    }

} // namespace Interstellar::Units
