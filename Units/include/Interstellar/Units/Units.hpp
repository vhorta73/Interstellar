#pragma once
/**
 * @file Units.hpp
 * @brief Interstellar Units facade over mp-units (>= 2.4.0) for consistent usage.
 * @ingroup Units
 */

 /// \defgroup Units Units (Interstellar)
 /// High-level, consistent wrappers and helpers built on top of mp-units.
 /// @{

 // v2.4 minimal style: one SI header + optional format/math
#include <mp-units/systems/si.h>
#include <mp-units/format.h>
#include <mp-units/math.h>

namespace Interstellar::Units {

    // Short aliases
    namespace mu = ::mp_units;
    namespace si = ::mp_units::si;
    namespace isq = ::mp_units::isq;

    // Opt-in symbols: Interstellar::Units::sym::m, ::s, ::Pa, ::K, ::km, ::m2, ::m3, ...
    namespace sym = ::mp_units::si::unit_symbols;

    // -----------------------------------------------------------------------------
    // Quantity aliases - deduced from unit expressions (rep fixed to double via 1.0)
    // -----------------------------------------------------------------------------
    using Length = decltype(1.0 * sym::m);
    using Area = decltype(1.0 * sym::m2);
    using Volume = decltype(1.0 * sym::m3);

    using Time = decltype(1.0 * sym::s);
    using Mass = decltype(1.0 * sym::kg);

    using Speed = decltype(1.0 * sym::m / sym::s);
    using Acceleration = decltype(1.0 * sym::m / (sym::s * sym::s));

    using Force = decltype(1.0 * sym::N);
    using Pressure = decltype(1.0 * sym::Pa);
    using Density = decltype(1.0 * sym::kg / sym::m3);

    using Energy = decltype(1.0 * sym::J);
    using Power = decltype(1.0 * sym::W);

    using TemperatureK = decltype(1.0 * sym::K);
    using Amount = decltype(1.0 * sym::mol);

    // NOTE: Intentionally no Celsius alias here - mp-units 2.4.0 symbol names vary by build.
    // Prefer Kelvin internally; convert at the UI layer if needed.

    /// @name Helpers
    /// @{

    /**
     * @brief Raw numeric value in the quantity's current unit.
     *
     * Prefer explicit `.in(target_unit)` or `std::format("{:N}", q)` for I/O.
     */
    template <class Q>
    constexpr auto num(const Q& q) noexcept { return q.number(); }

    /// @}

} // namespace Interstellar::Units

/// @}
