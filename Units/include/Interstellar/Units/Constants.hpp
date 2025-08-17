#ifndef INTERSTELLAR_UNITS_CONSTANTS_HPP
#define INTERSTELLAR_UNITS_CONSTANTS_HPP
#pragma once
/**
 * \file
 * \brief Physical constants and planetary references using mp-units (>= 2.4).
 * \defgroup units Units & Physical Constants
 * \details
 *   Consistent, strongly-typed SI quantities for simulation/gameplay code.
 *   All values are in **SI** and expressed as `mp-units` quantities.
 *   Sources noted per constant (CODATA 2018, IUGG mean radius, IERS GM).
 * \ingroup units
 */

#include <mp-units/systems/si.h> // header layout per mp-units v2+ examples
 // unit symbols are *opt-in*; keep them private to avoid leaking short ids
 // to users of this header. See mp-units docs on unit_symbols being opt-in.
namespace Interstellar::Units {
    inline namespace v1 {

        namespace detail {
            namespace sym = ::mp_units::si::unit_symbols;
        }

        // ----------------------- Base physical constants (SI) -----------------------

        /// \brief Standard gravity at sea level.
        /// \details Exact conventional value.\n
        /// **Units:** m/s^2. **Source:** conventional standard gravity.
        /// \ingroup units
        inline constexpr auto kStandardGravity =
            9.80665 * detail::sym::m / (detail::sym::s * detail::sym::s);

        /// \brief Standard atmospheric pressure at sea level.
        /// \details Conventional standard atmosphere (exact by definition in this context).\n
        /// **Units:** Pa.
        /// \ingroup units
        inline constexpr auto kStandardPressure = 101325.0 * detail::sym::Pa;

        /// \brief Universal (molar) gas constant R (CODATA 2018).
        /// \details Recommended value 8.31446261815324.\n
        /// **Units:** J/(mol*K).
        /// \ingroup units
        inline constexpr auto kGasConstantR =
            8.31446261815324 * detail::sym::J / (detail::sym::mol * detail::sym::K);

        /// \brief Stefan-Boltzmann constant (CODATA 2018).
        /// \details 5.670374419e-8.\n
        /// **Units:** W/(m^2*K^4).
        /// \ingroup units
        inline constexpr auto kStefanBoltzmann =
            5.670374419e-8 * detail::sym::W /
            (detail::sym::m2 * detail::sym::K * detail::sym::K * detail::sym::K * detail::sym::K);

        // ----------------------------- Earth references ----------------------------

        /// \brief Mean Earth radius (IUGG mean).
        /// \details 6,371,008.8 m.\n
        /// **Units:** m.
        /// \ingroup units
        inline constexpr auto kEarthRadius = 6'371'008.8 * detail::sym::m;

        /// \brief Earth mass (nominal).
        /// \details 5.9722x10^24 kg.\n
        /// **Units:** kg.
        /// \ingroup units
        inline constexpr auto kEarthMass = 5.9722e24 * detail::sym::kg;

        /// \brief Earth's standard gravitational parameter mu = G*M (IERS).
        /// \details 3.986004418x10^14 m^3/s^2.\n
        /// **Units:** m^3/s^2.
        /// \ingroup units
        inline constexpr auto kEarthMu =
            3.986004418e14 * detail::sym::m3 / (detail::sym::s * detail::sym::s);

        /// \brief Solar constant (top-of-atmosphere).
        /// \details Mean total solar irradiance at 1 AU; model with variability elsewhere.\n
        /// **Units:** W/m^2.
        /// \ingroup units
        inline constexpr auto kSolarConstant =
            1361.0 * detail::sym::W / detail::sym::m2;

        // ----------------------------- Compatibility -------------------------------
        // Deprecated old names (remove after call sites migrate).

        [[deprecated("Use kStandardGravity")]] inline constexpr auto g0 = kStandardGravity;
        [[deprecated("Use kStandardPressure")]] inline constexpr auto p0 = kStandardPressure;
        [[deprecated("Use kGasConstantR")]] inline constexpr auto R = kGasConstantR;
        [[deprecated("Use kStefanBoltzmann")]] inline constexpr auto sigma = kStefanBoltzmann;
        [[deprecated("Use kEarthRadius")]] inline constexpr auto EarthRadius = kEarthRadius;
        [[deprecated("Use kEarthMass")]] inline constexpr auto EarthMass = kEarthMass;
        [[deprecated("Use kEarthMu")]] inline constexpr auto GM_Earth = kEarthMu;
        [[deprecated("Use kSolarConstant")]] inline constexpr auto S0 = kSolarConstant;

    } // inline namespace v1
} // namespace Interstellar::Units

#endif // INTERSTELLAR_UNITS_CONSTANTS_HPP
