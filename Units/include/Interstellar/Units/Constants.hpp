#pragma once
/**
 * @file Constants.hpp
 * @brief Physical constants and planetary references (mp-units >= 2.4).
 * @ingroup Units
 */

#include <mp-units/systems/si.h>

namespace Interstellar::Units {

    // Short alias for unit symbols (opt-in, local only)
    namespace sym = ::mp_units::si::unit_symbols;

    // --- Base physical constants (SI) ---

    /// Standard gravity (exact definition)
    inline constexpr auto g0 = 9.80665 * sym::m / (sym::s * sym::s);

    /// Standard atmospheric pressure
    inline constexpr auto p0 = 101325.0 * sym::Pa;

    /// Universal gas constant (CODATA 2018)
    inline constexpr auto R = 8.31446261815324 * sym::J / (sym::mol * sym::K);

    /// Stefan-Boltzmann constant (CODATA 2018)
    inline constexpr auto sigma =
        5.670374419e-8 * sym::W / (sym::m2 * sym::K * sym::K * sym::K * sym::K);

    // --- Earth reference values ---

    /// Mean Earth radius (IUGG mean)
    inline constexpr auto EarthRadius = 6'371'008.8 * sym::m;

    /// Earth mass (nominal)
    inline constexpr auto EarthMass = 5.9722e24 * sym::kg;

    /// Earth standard gravitational parameter mu = G * M_earth (IERS)
    inline constexpr auto GM_Earth =
        3.986004418e14 * sym::m3 / (sym::s * sym::s);

} // namespace Interstellar::Units
