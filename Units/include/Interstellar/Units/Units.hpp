#pragma once
#ifndef INTERSTELLAR_UNITS_UNITS_HPP
#define INTERSTELLAR_UNITS_UNITS_HPP
/**
 * @file
 * @brief Single-file Interstellar Units facade (header-only, backend-agnostic API).
 * @details
 * Public API:
 *  - Types (PascalCase): Length, Time, Mass, Speed, Pressure, ...
 *  - Unit expressions:   Units::U::m, U::m2, U::m_per_s2, U::Pa, ...
 *  - Factories (lowercase domains):
 *        length::meters(v), length::square_meters(v), length::cubic_meters(v)
 *        time::seconds(v), time::minutes(v), time::hours(v)
 *        mass::kilograms(v), mass::grams(v), mass::tonnes(v)
 *        mech::pascals(v), mech::newtons(v), mech::joules(v), mech::watts(v),
 *             mech::meters_per_second(v), mech::meters_per_second2(v)
 *        thermo::K_delta(v), thermo::K_absolute(v), thermo::as_deltaK(x)
 *  - Utilities:           in(q, U::unit), qmath::sqrt(x)
 *
 * Notes:
 *  - All temperature arithmetic is performed on "delta K" (temperature differences).
 *  - Absolute Kelvin values are represented as quantity_point.
 *  - ASCII-only identifiers; no UTF-8 symbols.
 * @ingroup Units
 * @since 1.0
 */

#include <mp-units/systems/si.h>
#include <mp-units/math.h>

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

        /** @brief Length quantity (canonical unit: meter). @ingroup Units @since 1.0 */
        using Length = decltype(1.0 * detail::sym::m);

        /** @brief Area quantity (canonical unit: square meter). @ingroup Units @since 1.0 */
        using Area = decltype(1.0 * detail::sym::m2);

        /** @brief Volume quantity (canonical unit: cubic meter). @ingroup Units @since 1.0 */
        using Volume = decltype(1.0 * detail::sym::m3);

        /** @brief Time quantity (canonical unit: second). @ingroup Units @since 1.0 */
        using Time = decltype(1.0 * detail::sym::s);

        /** @brief Mass quantity (canonical unit: kilogram). @ingroup Units @since 1.0 */
        using Mass = decltype(1.0 * detail::sym::kg);

        /** @brief Amount of substance quantity (canonical unit: mole). @ingroup Units @since 1.0 */
        using Amount = decltype(1.0 * detail::sym::mol);

        /** @brief Speed quantity (canonical unit: meter per second). @ingroup Units @since 1.0 */
        using Speed = decltype(1.0 * detail::sym::m / detail::sym::s);

        /** @brief Acceleration quantity (canonical unit: meter per second squared). @ingroup Units @since 1.0 */
        using Acceleration = decltype(1.0 * detail::sym::m / (detail::sym::s * detail::sym::s));

        /** @brief Force quantity (canonical unit: newton). @ingroup Units @since 1.0 */
        using Force = decltype(1.0 * detail::sym::N);

        /** @brief Pressure quantity (canonical unit: pascal). @ingroup Units @since 1.0 */
        using Pressure = decltype(1.0 * detail::sym::Pa);

        /** @brief Density quantity (canonical unit: kilogram per cubic meter). @ingroup Units @since 1.0 */
        using Density = decltype(1.0 * detail::sym::kg / detail::sym::m3);

        /** @brief Energy quantity (canonical unit: joule). @ingroup Units @since 1.0 */
        using Energy = decltype(1.0 * detail::sym::J);

        /** @brief Power quantity (canonical unit: watt). @ingroup Units @since 1.0 */
        using Power = decltype(1.0 * detail::sym::W);

        /** @brief Temperature difference (delta K) as quantity (not a point). @ingroup Units @since 1.0 */
        using TemperatureDeltaK = decltype(detail::mu::delta<detail::sym::K>(1.0));

        /** @brief Absolute temperature in Kelvin as quantity_point. @ingroup Units @since 1.0 */
        using TemperaturePointK = decltype(detail::mu::absolute<detail::sym::K>(0.0));

        //========================
        // Unit-expression catalog (opt-in, no numbers)
        //========================
        /**
         * @brief Unit-expression catalog (qualified access only).
         * @details Use as U::m, U::m_per_s, U::Pa, etc. Contains units only (no numeric quantities).
         * @ingroup Units
         * @since 1.0
         */
        namespace U {
            // base units
            inline constexpr auto m = detail::sym::m;   ///< @ingroup Units
            inline constexpr auto s = detail::sym::s;   ///< @ingroup Units
            inline constexpr auto kg = detail::sym::kg;  ///< @ingroup Units
            inline constexpr auto mol = detail::sym::mol; ///< @ingroup Units
            inline constexpr auto K = detail::sym::K;   ///< @ingroup Units
            inline constexpr auto Pa = detail::sym::Pa;  ///< @ingroup Units
            inline constexpr auto one = detail::mu::one;  ///< Dimensionless. @ingroup Units

            // derived units
            inline constexpr auto m2 = detail::sym::m2;                                  ///< @ingroup Units
            inline constexpr auto m3 = detail::sym::m3;                                  ///< @ingroup Units
            inline constexpr auto m_per_s = detail::sym::m / detail::sym::s;                  ///< @ingroup Units
            inline constexpr auto m_per_s2 = detail::sym::m / (detail::sym::s * detail::sym::s); ///< @ingroup Units
            inline constexpr auto s2 = detail::sym::s * detail::sym::s;                  ///< @ingroup Units
            inline constexpr auto N = detail::sym::N;                                   ///< @ingroup Units
            inline constexpr auto J = detail::sym::J;                                   ///< @ingroup Units
            inline constexpr auto W = detail::sym::W;                                   ///< @ingroup Units

            // time convenience
            /** @brief One minute. @ingroup Units */
            inline constexpr auto minute = 60.0 * s;
            /** @brief One hour. @ingroup Units */
            inline constexpr auto hour = 60.0 * minute;
            /** @brief Alias for minute. @ingroup Units */
            inline constexpr auto min = minute;
            /** @brief Alias for hour. @ingroup Units */
            inline constexpr auto hr = hour;

            // composites
            /** @brief Watts per square meter. @ingroup Units */
            inline constexpr auto W_per_m2 = W / m2;
        } // namespace U

        //========================
        // Factories by domain
        //========================

        /// @brief Length factories (meters, km). @ingroup Units
        namespace length {
            /** @brief v meters. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto meters(double v)        noexcept -> Length { return v * U::m; }
            /** @brief v square meters. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto square_meters(double v) noexcept -> Area { return v * U::m2; }
            /** @brief v cubic meters. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto cubic_meters(double v)  noexcept -> Volume { return v * U::m3; }

            /** @brief One kilometer in meters. @ingroup Units @since 1.0 */
            inline constexpr auto km = 1000.0 * U::m;
            /** @brief v kilometers. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto kilometers(double v) noexcept -> Length { return v * km; }
        }

        /// @brief Time factories (seconds, minutes, hours). @ingroup Units
        namespace time {
            /** @brief v seconds. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto seconds(double v) noexcept -> Time { return v * U::s; }
            /** @brief v minutes. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto minutes(double v) noexcept -> Time { return v * U::minute; }
            /** @brief v hours. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto hours(double v)   noexcept -> Time { return v * U::hour; }
        }

        /// @brief Mass factories (kg, g, t). @ingroup Units
        namespace mass {
            /** @brief v kilograms. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto kilograms(double v) noexcept -> Mass { return v * U::kg; }
            /** @brief v grams. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto grams(double v)     noexcept -> Mass { return v * (1.0 / 1000.0) * U::kg; }
            /** @brief v tonnes. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto tonnes(double v)    noexcept -> Mass { return v * (1000.0 * U::kg); }
        }

        /// @brief Mechanics factories (speed, accel, force, energy, power, pressure). @ingroup Units
        namespace mech {
            /** @brief v meters per second. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto meters_per_second(double v)  noexcept -> Speed { return v * U::m_per_s; }
            /** @brief v meters per second squared. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto meters_per_second2(double v) noexcept -> Acceleration { return v * U::m_per_s2; }
            /** @brief v newtons. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto newtons(double v)            noexcept -> Force { return v * U::N; }
            /** @brief v joules. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto joules(double v)             noexcept -> Energy { return v * U::J; }
            /** @brief v watts. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto watts(double v)              noexcept -> Power { return v * U::W; }
            /** @brief v pascals. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto pascals(double v)            noexcept -> Pressure { return v * U::Pa; }
        }

        /// @brief Thermodynamics helpers (Kelvin-safe). @ingroup Units
        namespace thermo {
            /** @brief Construct a temperature difference (delta K). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto K_delta(double v) noexcept -> TemperatureDeltaK {
                return detail::mu::delta<detail::sym::K>(v);
            }

            /** @brief Construct an absolute temperature point in Kelvin. @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr auto K_absolute(double v) noexcept -> TemperaturePointK {
                return detail::mu::absolute<detail::sym::K>(v);
            }

            /** @brief Convert absolute K (point) or delta K (quantity) to delta-K. @ingroup Units @since 1.0 */
            template <detail::mu::QuantityPoint T>
            [[nodiscard]] constexpr auto as_deltaK(const T& t)
                noexcept(noexcept(t - detail::si::absolute_zero))
            {
                return t - detail::si::absolute_zero;
            }

            /** @brief Overload for quantities already in delta-K. @ingroup Units @since 1.0 */
            template <class T>
                requires (!detail::mu::QuantityPoint<T>)
            [[nodiscard]] constexpr auto as_deltaK(const T& t) noexcept {
                return t;
            }

            /**
             * @brief Numeric extraction: return delta-K as a scalar.
             * @tparam T Any temperature-difference quantity convertible via division by delta<K>.
             * @return Value in K (difference).
             * @ingroup Units
             * @since 1.0
             */
            template<class T>
                requires requires (const T& x) {
                static_cast<double>(x / detail::mu::delta<detail::sym::K>(1.0));
            }
            [[nodiscard]] inline double in_K(const T& t)
                noexcept(noexcept(t / detail::mu::delta<detail::sym::K>(1.0)))
            {
                return static_cast<double>(t / detail::mu::delta<detail::sym::K>(1.0));
            }

            /**
             * @brief Numeric extraction for absolute temperature points (converts to delta-K first).
             * @ingroup Units
             * @since 1.0
             */
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
        /// @brief Quantity-aware square root (for quantities and scalars). @ingroup Units @since 1.0
        namespace qmath {
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
         * @tparam Q Quantity type.
         * @tparam UnitExpr Unit expression (for example U::m, U::m_per_s).
         * @param q Quantity to convert.
         * @param u Unit to express q in.
         * @return Value of q in units of u.
         * @par Example
         * @code
         * Speed v = 250.0 * U::m_per_s;
         * double v_ms = in(v, U::m_per_s); // 250.0
         * @endcode
         * @ingroup Units
         * @since 1.0
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
        /**
         * @brief User-defined literals for common quantities (opt-in).
         * @details Enable with `using namespace Interstellar::Units::literals;`.
         * @ingroup Units
         * @since 1.0
         */
        namespace literals {

            // Amount
            /** @brief amount in mol (long double). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Amount operator"" _mol(long double v)        noexcept { return static_cast<double>(v) * U::mol; }
            /** @brief amount in mol (unsigned long long). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Amount operator"" _mol(unsigned long long v) noexcept { return static_cast<double>(v) * U::mol; }

            // Length
            /** @brief length in meters (long double). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Length operator"" _m(long double v)        noexcept { return static_cast<double>(v) * U::m; }
            /** @brief length in meters (unsigned long long). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Length operator"" _m(unsigned long long v) noexcept { return static_cast<double>(v) * U::m; }

            // Area / Volume
            /** @brief area in m^2 (long double). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Area   operator"" _m2(long double v)        noexcept { return static_cast<double>(v) * U::m2; }
            /** @brief area in m^2 (unsigned long long). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Area   operator"" _m2(unsigned long long v) noexcept { return static_cast<double>(v) * U::m2; }
            /** @brief volume in m^3 (long double). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Volume operator"" _m3(long double v)        noexcept { return static_cast<double>(v) * U::m3; }
            /** @brief volume in m^3 (unsigned long long). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Volume operator"" _m3(unsigned long long v) noexcept { return static_cast<double>(v) * U::m3; }

            // Time
            /** @brief time in seconds (long double). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Time operator"" _s(long double v)        noexcept { return static_cast<double>(v) * U::s; }
            /** @brief time in seconds (unsigned long long). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Time operator"" _s(unsigned long long v) noexcept { return static_cast<double>(v) * U::s; }

            // Mass
            /** @brief mass in kg (long double). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Mass operator"" _kg(long double v)        noexcept { return static_cast<double>(v) * U::kg; }
            /** @brief mass in kg (unsigned long long). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Mass operator"" _kg(unsigned long long v) noexcept { return static_cast<double>(v) * U::kg; }

            // Pressure
            /** @brief pressure in Pa (long double). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Pressure operator"" _Pa(long double v)        noexcept { return static_cast<double>(v) * U::Pa; }
            /** @brief pressure in Pa (unsigned long long). @ingroup Units @since 1.0 */
            [[nodiscard]] constexpr Pressure operator"" _Pa(unsigned long long v) noexcept { return static_cast<double>(v) * U::Pa; }

        } // namespace literals

    } // inline namespace v1
} // namespace Interstellar::Units

#endif // INTERSTELLAR_UNITS_UNITS_HPP
