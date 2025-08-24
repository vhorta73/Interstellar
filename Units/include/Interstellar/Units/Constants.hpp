#ifndef INTERSTELLAR_UNITS_CONSTANTS_HPP
#define INTERSTELLAR_UNITS_CONSTANTS_HPP
#pragma once
/**
 * @ingroup Units
 * @brief Physical constants and planetary references using mp-units (>= 2.4).
 * @details
 *   Consistent, strongly-typed SI quantities for simulation/gameplay code.
 *   All values are in SI and expressed as mp-units quantities.
 *   Sources noted per constant (CODATA 2018, IUGG mean radius, IERS GM).
 */

#include <mp-units/systems/si.h>

namespace Interstellar::Units {
    inline namespace v1 {

        namespace detail {
            namespace sym = ::mp_units::si::unit_symbols;
        }

        /* ----------------------- Base physical constants (SI) ----------------------- */

        /**
         * @brief Standard gravity at sea level (exact conventional value).
         * @details Units: m/s^2.
         * @ingroup Units
         * @since 1.0
         */
        inline constexpr auto kStandardGravity =
            9.80665 * detail::sym::m / (detail::sym::s * detail::sym::s);

        /**
         * @brief Standard atmospheric pressure at sea level (conventional).
         * @details Units: Pa.
         * @ingroup Units
         * @since 1.0
         */
        inline constexpr auto kStandardPressure = 101325.0 * detail::sym::Pa;

        /**
         * @brief Universal (molar) gas constant R (CODATA 2018).
         * @details Units: J/(mol*K).
         * @ingroup Units
         * @since 1.0
         */
        inline constexpr auto kGasConstantR =
            8.31446261815324 * detail::sym::J / (detail::sym::mol * detail::sym::K);

        /**
         * @brief Stefan-Boltzmann constant (CODATA 2018, 5.670374419e-8).
         * @details Units: W/(m^2*K^4).
         * @ingroup Units
         * @since 1.0
         */
        inline constexpr auto kStefanBoltzmann =
            5.670374419e-8 * detail::sym::W /
            (detail::sym::m2 * detail::sym::K * detail::sym::K * detail::sym::K * detail::sym::K);

        /* ----------------------------- Earth references ---------------------------- */

        /**
         * @brief Mean Earth radius (IUGG mean, 6,371,008.8 m).
         * @details Units: m.
         * @ingroup Units
         * @since 1.0
         */
        inline constexpr auto kEarthRadius = 6'371'008.8 * detail::sym::m;

        /**
         * @brief Earth mass (nominal, 5.9722e24 kg).
         * @details Units: kg.
         * @ingroup Units
         * @since 1.0
         */
        inline constexpr auto kEarthMass = 5.9722e24 * detail::sym::kg;

        /**
         * @brief Earth GM (IERS, 3.986004418e14).
         * @details Units: m^3/s^2.
         * @ingroup Units
         * @since 1.0
         */
        inline constexpr auto kEarthMu =
            3.986004418e14 * detail::sym::m3 / (detail::sym::s * detail::sym::s);

        /**
         * @brief Solar constant (mean TOA irradiance, about 1361).
         * @details Units: W/m^2.
         * @ingroup Units
         * @since 1.0
         */
        inline constexpr auto kSolarConstant =
            1361.0 * detail::sym::W / detail::sym::m2;

    } // inline namespace v1
} // namespace Interstellar::Units

#endif // INTERSTELLAR_UNITS_CONSTANTS_HPP
