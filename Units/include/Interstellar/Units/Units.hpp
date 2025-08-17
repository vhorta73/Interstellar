#pragma once
#ifndef INTERSTELLAR_UNITS_UNITS_HPP
#define INTERSTELLAR_UNITS_UNITS_HPP
/**
 * @file
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
 *  - Utilities:           in(q, U::unit), in_K(t_delta), in_K_absolute(t_point)
 *
 * Notes:
 *  - All temperature arithmetic is performed on "delta K" (temperature differences).
 *  - Absolute Kelvin values are represented as quantity_point.
 *  - This header avoids UTF-8 symbols; names like "delta" and "mu" are used in text.
 */

#include <mp-units/systems/si.h>
#include <mp-units/math.h>     // keep format.h out of the facade to reduce compile time

namespace Interstellar::Units {
    inline namespace v1 {

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

        /// @brief Length quantity (canonical unit: meter).
        using Length = decltype(1.0 * detail::sym::m);

        /// @brief Area quantity (canonical unit: square meter).
        using Area = decltype(1.0 * detail::sym::m2);

        /// @brief Volume quantity (canonical unit: cubic meter).
        using Volume = decltype(1.0 * detail::sym::m3);

        /// @brief Time quantity (canonical unit: second).
        using Time = decltype(1.0 * detail::sym::s);

        /// @brief Mass quantity (canonical unit: kilogram).
        using Mass = decltype(1.0 * detail::sym::kg);

        /// @brief Amount of substance quantity (canonical unit: mole).
        using Amount = decltype(1.0 * detail::sym::mol);

        /// @brief Speed quantity (canonical unit: meter per second).
        using Speed = decltype(1.0 * detail::sym::m / detail::sym::s);

        /// @brief Acceleration quantity (canonical unit: meter per second squared).
        using Acceleration = decltype(1.0 * detail::sym::m / (detail::sym::s * detail::sym::s));

        /// @brief Force quantity (canonical unit: newton).
        using Force = decltype(1.0 * detail::sym::N);

        /// @brief Pressure quantity (canonical unit: pascal).
        using Pressure = decltype(1.0 * detail::sym::Pa);

        /// @brief Density quantity (canonical unit: kilogram per cubic meter).
        using Density = decltype(1.0 * detail::sym::kg / detail::sym::m3);

        /// @brief Energy quantity (canonical unit: joule).
        using Energy = decltype(1.0 * detail::sym::J);

        /// @brief Power quantity (canonical unit: watt).
        using Power = decltype(1.0 * detail::sym::W);

        /// @brief Temperature difference (delta K) as quantity (not a point).
        using TemperatureDeltaK = decltype(detail::mu::delta   <detail::sym::K>(1.0));

        /// @brief Absolute temperature in Kelvin as quantity_point.
        using TemperaturePointK = decltype(detail::mu::absolute<detail::sym::K>(0.0));

        //========================
        /* Unit-expression catalog (opt-in, no numbers)
           Qualified access only: U::m, U::m2, U::m_per_s, ...
           This avoids leaking single-letter names into client namespaces. */
           //========================
        namespace U {
            // base units
            inline constexpr auto m = detail::sym::m;
            inline constexpr auto s = detail::sym::s;
            inline constexpr auto kg = detail::sym::kg;
            inline constexpr auto mol = detail::sym::mol;
            inline constexpr auto K = detail::sym::K;
            inline constexpr auto Pa = detail::sym::Pa;
            inline constexpr auto one = detail::mu::one;

            // derived units
            inline constexpr auto m2 = detail::sym::m2;
            inline constexpr auto m3 = detail::sym::m3;
            inline constexpr auto m_per_s = detail::sym::m / detail::sym::s;
            inline constexpr auto m_per_s2 = detail::sym::m / (detail::sym::s * detail::sym::s);
            inline constexpr auto s2 = detail::sym::s * detail::sym::s;
            inline constexpr auto N = detail::sym::N;
            inline constexpr auto J = detail::sym::J;
            inline constexpr auto W = detail::sym::W;

            // time convenience
            inline constexpr auto minute = 60.0 * s;
            inline constexpr auto hour = 60.0 * minute;
            inline constexpr auto min = minute;
            inline constexpr auto hr = hour;

            // composites
            inline constexpr auto W_per_m2 = W / m2;
        } // namespace U

        //========================
        // Factories by domain
        //========================

        // Length / Area / Volume
        namespace length {
            [[nodiscard]] constexpr auto meters(double v)        noexcept -> Length { return v * U::m; }
            [[nodiscard]] constexpr auto square_meters(double v) noexcept -> Area { return v * U::m2; }
            [[nodiscard]] constexpr auto cubic_meters(double v)  noexcept -> Volume { return v * U::m3; }

            inline constexpr auto km = 1000.0 * U::m;
            [[nodiscard]] constexpr auto kilometers(double v) noexcept -> Length { return v * km; }
        }

        // Time
        namespace time {
            [[nodiscard]] constexpr auto seconds(double v) noexcept -> Time { return v * U::s; }
            [[nodiscard]] constexpr auto minutes(double v) noexcept -> Time { return v * U::minute; }
            [[nodiscard]] constexpr auto hours(double v)   noexcept -> Time { return v * U::hour; }
        }

        // Mass
        namespace mass {
            [[nodiscard]] constexpr auto kilograms(double v) noexcept -> Mass { return v * U::kg; }
            [[nodiscard]] constexpr auto grams(double v)     noexcept -> Mass { return v * (1.0 / 1000.0) * U::kg; }
            [[nodiscard]] constexpr auto tonnes(double v)    noexcept -> Mass { return v * (1000.0 * U::kg); }
        }

        // Mechanics (speed, accel, force, energy, power, pressure)
        namespace mech {
            [[nodiscard]] constexpr auto meters_per_second(double v)  noexcept -> Speed { return v * U::m_per_s; }
            [[nodiscard]] constexpr auto meters_per_second2(double v) noexcept -> Acceleration { return v * U::m_per_s2; }
            [[nodiscard]] constexpr auto newtons(double v)            noexcept -> Force { return v * U::N; }
            [[nodiscard]] constexpr auto joules(double v)             noexcept -> Energy { return v * U::J; }
            [[nodiscard]] constexpr auto watts(double v)              noexcept -> Power { return v * U::W; }
            [[nodiscard]] constexpr auto pascals(double v)            noexcept -> Pressure { return v * U::Pa; }
        }

        // Thermodynamics (Kelvin - offset safe)
        namespace thermo {
            /// @brief Construct a temperature difference (delta K).
            [[nodiscard]] constexpr auto K_delta(double v) noexcept -> TemperatureDeltaK {
                return detail::mu::delta<detail::sym::K>(v);
            }

            /// @brief Construct an absolute temperature point in Kelvin.
            [[nodiscard]] constexpr auto K_absolute(double v) noexcept -> TemperaturePointK {
                return detail::mu::absolute<detail::sym::K>(v);
            }

            // For quantity_point<T>: subtract absolute zero to get a delta-K.
            template <detail::mu::QuantityPoint T>
            [[nodiscard]] constexpr auto as_deltaK(const T& t)
                noexcept(noexcept(t - detail::si::absolute_zero))
            {
                return t - detail::si::absolute_zero;
            }

            // For quantity (already a delta): just forward it.
            template <class T>
                requires (!detail::mu::QuantityPoint<T>)
            [[nodiscard]] constexpr auto as_deltaK(const T& t) noexcept {
                return t;
            }

            // Accept any temperature *difference* for which the division by delta<K> is castable to double.
            template<class T>
                requires requires (const T& x) {
                static_cast<double>(x / detail::mu::delta<detail::sym::K>(1.0));
            }
            [[nodiscard]] inline double in_K(const T& t)
                noexcept(noexcept(t / detail::mu::delta<detail::sym::K>(1.0)))
            {
                return static_cast<double>(t / detail::mu::delta<detail::sym::K>(1.0));
            }

            // Overload for absolute temperature *points*
            template<class T>
                requires (detail::mu::QuantityPoint<T>)
            [[nodiscard]] inline double in_K(const T& t_abs)
            {
                const auto dT = t_abs - detail::si::absolute_zero;
                return in_K(dT);
            }
        }

        //========================
        // Quantity-aware math (optional)
        //========================
        namespace qmath {
            /// @brief Quantity-aware square root (for quantities and scalars).
            template<class Q>
            [[nodiscard]] constexpr auto sqrt(const Q& x) noexcept(noexcept(detail::mu::sqrt(x))) {
                return detail::mu::sqrt(x);
            }
        }

        //========================
        // Scalar extraction utility
        //========================

        /**
         * @brief Return a quantity as a scalar in the given unit.
         * @tparam Rep Optional target scalar representation (defaults to double).
         * @param q    Quantity to convert.
         * @param u    Unit expression (for example U::m, U::m_per_s).
         * @return Value of q in units of u.
         *
         * Example:
         * @code
         * Speed v = 250.0 * U::m_per_s;
         * double v_ms = in(v, U::m_per_s); // 250.0
         * @endcode
         */
        template<class Rep = double, class Q, class UnitExpr>
        [[nodiscard]] constexpr Rep in(const Q& q, UnitExpr u)
            noexcept(noexcept(q / (Rep{ 1 } *u)))
        {
            return static_cast<Rep>(q / (Rep{ 1 } *u));
        }

        //========================
        // User-defined literals (opt-in)
        //========================
        namespace literals {

            // Amount
            [[nodiscard]] constexpr Amount operator"" _mol(long double v)        noexcept { return static_cast<double>(v) * U::mol; }
            [[nodiscard]] constexpr Amount operator"" _mol(unsigned long long v) noexcept { return static_cast<double>(v) * U::mol; }

            // Length
            [[nodiscard]] constexpr Length operator"" _m(long double v)        noexcept { return static_cast<double>(v) * U::m; }
            [[nodiscard]] constexpr Length operator"" _m(unsigned long long v) noexcept { return static_cast<double>(v) * U::m; }

            // Area / Volume
            [[nodiscard]] constexpr Area   operator"" _m2(long double v)        noexcept { return static_cast<double>(v) * U::m2; }
            [[nodiscard]] constexpr Area   operator"" _m2(unsigned long long v) noexcept { return static_cast<double>(v) * U::m2; }
            [[nodiscard]] constexpr Volume operator"" _m3(long double v)        noexcept { return static_cast<double>(v) * U::m3; }
            [[nodiscard]] constexpr Volume operator"" _m3(unsigned long long v) noexcept { return static_cast<double>(v) * U::m3; }

            // Time
            [[nodiscard]] constexpr Time operator"" _s(long double v)        noexcept { return static_cast<double>(v) * U::s; }
            [[nodiscard]] constexpr Time operator"" _s(unsigned long long v) noexcept { return static_cast<double>(v) * U::s; }

            // Mass
            [[nodiscard]] constexpr Mass operator"" _kg(long double v)        noexcept { return static_cast<double>(v) * U::kg; }
            [[nodiscard]] constexpr Mass operator"" _kg(unsigned long long v) noexcept { return static_cast<double>(v) * U::kg; }

            // Pressure
            [[nodiscard]] constexpr Pressure operator"" _Pa(long double v)        noexcept { return static_cast<double>(v) * U::Pa; }
            [[nodiscard]] constexpr Pressure operator"" _Pa(unsigned long long v) noexcept { return static_cast<double>(v) * U::Pa; }

        } // namespace literals

    } // inline namespace v1
} // namespace Interstellar::Units

#endif // INTERSTELLAR_UNITS_UNITS_HPP
