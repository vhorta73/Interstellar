#pragma once

#include <cmath>

/**
 * @file Math.hpp
 * @brief Small, unit-safe helpers used by gameplay systems.
 *        All temperature math is performed on delta-K (temperature differences).
 *        Callers may pass absolute K (quantity_point) or delta K (quantity);
 *        we normalize to delta K internally via Thermo::as_deltaK.
 */

#include "Interstellar/Units/Units.hpp"        // aggregator: Core + domain factories
#include "Interstellar/Units/Constants.hpp"    // R, sigma, GM_Earth, etc.

namespace Interstellar::Units {

    /** @brief Ideal gas law: P = n * R * T / V
     * Accepts T as absolute K or delta K; internally uses delta K to avoid offset-unit ops.
     * Returns pressure in Pa for SI inputs.
     */
    template <class AmountQ, class TempQ, class VolQ>
    constexpr auto ideal_gas_pressure(const AmountQ& n, const TempQ& tempK_any, const VolQ& vol) {
        const auto dT = thermo::as_deltaK(tempK_any);    // delta-K
        return (n * R * dT) / vol;                       // Pressure (Pa)
    }

    /** @brief Hydrostatic pressure head: delta-P = rho * g * h */
    template <class DensityQ, class AccelQ, class LengthQ>
    constexpr auto hydrostatic_pressure(const DensityQ& rho, const AccelQ& accel, const LengthQ& depth) {
        return rho * accel * depth;                      // Pressure (Pa)
    }

    /** @brief Escape velocity: v_e = sqrt( 2 * mu / r ), where mu = G*M */
    template <class MuQ, class LengthQ>
    inline auto escape_velocity(const MuQ& mu, const LengthQ& radius) {
        return qmath::sqrt(2.0 * mu / radius);           // Speed (m/s)
    }

    /** @brief No-greenhouse radiative equilibrium:
     *         T = ((1 - A) * S / (4 * sigma))^(1/4)
     * Returns a Kelvin quantity (delta-K).
     */
    template <class FluxQ>
    inline auto equilibrium_temp(const FluxQ& S, double albedo) {
        const auto x = ((1.0 - albedo) * S) / (4.0 * ::Interstellar::Units::sigma);
        // mp-units 2.4: prefer sqrt(sqrt(x)) over pow(x, 0.25)
        return qmath::sqrt(qmath::sqrt(x));              // Temperature (delta-K)
    }

    // --- Tetens saturation vapor pressure ---
    // e_s = C0 * exp( A * Tc / (Tc + B) )
    // Tc uses differences in C (numerically equal to K differences).
    template<class TempQ>
    inline auto saturation_vapor_pressure_tetens(const TempQ& tempK_any) {
        const auto dT = thermo::as_deltaK(tempK_any); // delta-K
        const auto Tc = dT - thermo::K_delta(273.15); // delta-K; numeric == C
        const auto tetens_B = thermo::K_delta(237.3);       // delta-K

        constexpr double tetens_A = 17.27;   // dimensionless
        constexpr double tetens_C0 = 610.78;  // Pa (scalar)

        const double frac = static_cast<double>(Tc / (Tc + tetens_B)); // dimensionless
        const double expo = tetens_A * frac;

        return (tetens_C0 * std::exp(expo)) * U::Pa;       // Pressure (Pa)
    }

    // Convenience overload: numeric Kelvin input (construct as delta-K for the empirical formula)
    inline auto saturation_vapor_pressure_tetens_K(double T_numK) {
        return saturation_vapor_pressure_tetens(thermo::K_delta(T_numK));
    }

    /** @brief Relative humidity (0..1) to partial pressure: e = RH * e_s */
    template <class PressureQ>
    constexpr auto vapor_partial_pressure(double RH, const PressureQ& e_s) {
        return RH * e_s;                                   // Pressure (Pa)
    }

} // namespace Interstellar::Units
