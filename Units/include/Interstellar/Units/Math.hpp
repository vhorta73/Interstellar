#pragma once
/**
 * @file Math.hpp
 * @brief Small, unit-safe helpers used by gameplay systems (mp-units >= 2.4).
 * @ingroup Units
 */

#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Constants.hpp"

namespace Interstellar::Units {

    // Opt-in symbols: Interstellar::Units::sym::m, ::s, ::Pa, ::K, ::km, ::m2, ::m3, ...
    namespace sym = ::mp_units::si::unit_symbols;

    /** @brief Ideal gas law: P = rho R T / V */
    template <class AmountQ, class TempQ, class VolQ>
    constexpr auto ideal_gas_pressure(const AmountQ& n, const TempQ& tempK, const VolQ& vol) {
        return (n * R * tempK) / vol; // Pa with SI inputs
    }

    ///** @brief Hydrostatic pressure head: deltaP = p g h */
    template <class DensityQ, class AccelQ, class LengthQ>
    constexpr auto hydrostatic_pressure(const DensityQ& rho, const AccelQ& accel, const LengthQ& depth) {
        return rho * accel * depth; // Pa
    }

    /** @brief Escape velocity: v_e = sqrt(2 mu / r) where mu = GM */
    template <class MuQ, class LengthQ>
    inline auto escape_velocity(const MuQ& mu, const LengthQ& radius) {
        return ::mp_units::sqrt(2.0 * mu / radius); // Speed
    }

    /** @brief No-greenhouse radiative equilibrium: T = ((1-A) S / (4sigma))^(1/4) */
    template <class FluxQ>
    inline auto equilibrium_temp(const FluxQ& S, double A) {
        const auto x = ((1.0 - A) * S) / (4.0 * ::Interstellar::Units::sigma); // <-- qualify sigma
        // mp-units 2.4: use sqrt(sqrt(x)) instead of pow(x, 0.25)
        return ::mp_units::sqrt(::mp_units::sqrt(x)); // Kelvin
    }

} // namespace Interstellar::Units
