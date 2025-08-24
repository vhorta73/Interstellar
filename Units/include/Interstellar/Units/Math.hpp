#ifndef INTERSTELLAR_UNITS_MATH_HPP
#define INTERSTELLAR_UNITS_MATH_HPP
#pragma once

#include <cmath>
#include <cassert>

/**
 * @file
 * @ingroup Units
 * @brief Small, unit-safe helpers used by gameplay systems.
 * @details
 *   All temperature math is performed on delta-K (temperature differences).
 *   Callers may pass absolute K (quantity_point) or delta K (quantity);
 *   we normalize to delta K internally via thermo::as_deltaK.
 *
 * External expectations:
 *   - thermo::as_deltaK(T): converts absolute or delta K to a delta-K quantity.
 *   - thermo::K_delta(x): constructs a delta-K quantity from a numeric scalar.
 */

#include "Interstellar/Units/Units.hpp"        // aliases + U:: catalog
#include "Interstellar/Units/Constants.hpp"    // kGasConstantR, kStefanBoltzmann, etc.
#include <mp-units/math.h>                     // sqrt for quantities

namespace Interstellar::Units {
    inline namespace v1 {

        //--------------------------------------------------------------------------
        // Ideal gas law: P = n * R * T / V
        //--------------------------------------------------------------------------

        /**
         * @brief Ideal gas law: P = (n * R * T) / V.
         * @tparam AmountQ  quantity compatible with amount of substance (mol).
         * @tparam TempQ    absolute K (quantity_point) or delta-K (quantity).
         * @tparam VolQ     quantity compatible with volume (m^3).
         * @param n         Amount of substance (mol).
         * @param tempK_any Absolute or delta Kelvin.
         * @param vol       Volume (m^3), must be > 0.
         * @return Pressure (Pa).
         * @pre vol > 0.
         * @note Uses kGasConstantR (CODATA 2018). Temperature is treated as delta-K.
         * @ingroup Units
         * @since 1.0
         */
        template <class AmountQ, class TempQ, class VolQ>
        [[nodiscard]] constexpr auto ideal_gas_pressure(const AmountQ& n,
            const TempQ& tempK_any,
            const VolQ& vol)
            noexcept(noexcept((n* kGasConstantR* thermo::as_deltaK(tempK_any)) / vol))
            requires requires {
            thermo::as_deltaK(tempK_any);
            (n* kGasConstantR* thermo::as_deltaK(tempK_any)) / vol;
        }
        {
            assert(in(vol, U::m3) > 0.0 && "ideal_gas_pressure: volume must be > 0");
            const auto dT = thermo::as_deltaK(tempK_any);   // delta-K
            return (n * kGasConstantR * dT) / vol;          // Pa
        }

        //--------------------------------------------------------------------------
        // Hydrostatic head: delta-P = rho * g * h
        //--------------------------------------------------------------------------

        /**
         * @brief Hydrostatic pressure head: delta-P = rho * g * h.
         * @tparam DensityQ density quantity (kg/m^3).
         * @tparam AccelQ   acceleration (m/s^2).
         * @tparam LengthQ  depth/height (m).
         * @param rho       Fluid density (kg/m^3).
         * @param accel     Gravitational acceleration (m/s^2).
         * @param depth     Column depth or height (m), must be >= 0.
         * @return Pressure (Pa).
         * @pre depth >= 0.
         * @ingroup Units
         * @since 1.0
         */
        template <class DensityQ, class AccelQ, class LengthQ>
        [[nodiscard]] constexpr auto hydrostatic_pressure(const DensityQ& rho,
            const AccelQ& accel,
            const LengthQ& depth)
            noexcept(noexcept(rho* accel* depth))
            requires requires { rho* accel* depth; }
        {
            assert(in(depth, U::m) >= 0.0 && "hydrostatic_pressure: depth must be >= 0");
            return rho * accel * depth; // Pa
        }

        //--------------------------------------------------------------------------
        // Escape velocity: v_e = sqrt(2 * mu / r)
        //--------------------------------------------------------------------------

        /**
         * @brief Escape velocity: v = sqrt(2 * mu / r), with mu = G * M.
         * @tparam MuQ     standard gravitational parameter (m^3/s^2).
         * @tparam LengthQ radius (m).
         * @param mu       Gravitational parameter (m^3/s^2).
         * @param radius   Distance from center (m), must be > 0.
         * @return Speed (m/s).
         * @pre radius > 0.
         * @ingroup Units
         * @since 1.0
         */
        template <class MuQ, class LengthQ>
        [[nodiscard]] constexpr auto escape_velocity(const MuQ& mu, const LengthQ& radius)
            noexcept(noexcept(::mp_units::sqrt(2.0 * mu / radius)))
            requires requires { ::mp_units::sqrt(2.0 * mu / radius); }
        {
            assert(in(radius, U::m) > 0.0 && "escape_velocity: radius must be > 0");
            return ::mp_units::sqrt(2.0 * mu / radius);
        }

        //--------------------------------------------------------------------------
        // No-greenhouse radiative equilibrium:
        // T = ((1 - A) * S / (4 * sigma))^(1/4)
        //--------------------------------------------------------------------------

        /**
         * @brief No-greenhouse radiative equilibrium temperature (delta-K).
         * @details T = ((1 - A) * S / (4 * sigma))^(1/4).
         * @tparam FluxQ   irradiance quantity (W/m^2).
         * @param S        Stellar flux (W/m^2).
         * @param albedo   Bond albedo in [0, 1].
         * @return Temperature difference (delta-K).
         * @pre 0.0 <= albedo <= 1.0.
         * @note Returns a delta-K quantity per project convention.
         * @ingroup Units
         * @since 1.0
         */
        template <class FluxQ>
        [[nodiscard]] constexpr auto equilibrium_temp(const FluxQ& S, double albedo)
            noexcept(noexcept(::mp_units::sqrt(::mp_units::sqrt(((1.0 - albedo)* S) / (4.0 * kStefanBoltzmann))))))
            requires requires { ((1.0 - albedo)* S) / (4.0 * kStefanBoltzmann); }
            {
                assert(albedo >= 0.0 && albedo <= 1.0 && "equilibrium_temp: albedo must be in [0,1]");
                const auto x = ((1.0 - albedo) * S) / (4.0 * kStefanBoltzmann);
                // mp-units: prefer sqrt(sqrt(x)) to model the fourth root without pow
                return ::mp_units::sqrt(::mp_units::sqrt(x));
        }

            /**
             * @brief Absolute (Kelvin) equilibrium temperature as a quantity point.
             * @details Uses the same expression as equilibrium_temp and returns K as a quantity_point.
             * @tparam FluxQ      irradiance quantity (W/m^2).
             * @tparam KelvinPoint a constructible temperature point type from your backend.
             * @param S           Stellar flux (W/m^2).
             * @param albedo      Bond albedo in [0, 1].
             * @return Temperature point (K).
             * @ingroup Units
             * @since 1.0
             */
            template <class FluxQ, class KelvinPoint = decltype(0.0 * U::K + 0.0 * U::K)>
            [[nodiscard]] constexpr auto equilibrium_temp_point(const FluxQ& S, double albedo)
                noexcept(noexcept(KelvinPoint{ equilibrium_temp(S, albedo) }))
            {
                return KelvinPoint{ equilibrium_temp(S, albedo) };
            }

            //--------------------------------------------------------------------------
            // Tetens saturation vapor pressure
            // e_s = C0 * exp( A * Tc / (Tc + B) ), with Tc in C (delta-K numerically)
            //--------------------------------------------------------------------------

            /**
             * @brief Tetens saturation vapor pressure (Pa).
             * @details e_s = C0 * exp( A * Tc / (Tc + B) ). Tc is in Celsius; implemented via
             *          delta-K with the usual numeric equivalence.
             * @tparam TempQ   absolute K (quantity_point) or delta-K (quantity).
             * @param tempK_any Absolute or delta Kelvin.
             * @return Saturation vapor pressure (Pa).
             * @pre (Tc + B) != 0.
             * @ingroup Units
             * @since 1.0
             */
            template<class TempQ>
            [[nodiscard]] inline auto saturation_vapor_pressure_tetens(const TempQ& tempK_any)
                noexcept(noexcept(thermo::as_deltaK(tempK_any)))
                requires requires { thermo::as_deltaK(tempK_any); }
            {
                const auto dT = thermo::as_deltaK(tempK_any);   // delta-K
                const auto Tc = dT - thermo::K_delta(273.15);   // "C" as delta-K
                const auto tetens_B = thermo::K_delta(237.3);   // delta-K

                constexpr double tetens_A = 17.27;              // dimensionless
                constexpr double tetens_C0 = 610.78;            // Pa (scalar)

                const double frac = static_cast<double>(Tc / (Tc + tetens_B)); // dimensionless
                const double expo = tetens_A * frac;

                return (tetens_C0 * std::exp(expo)) * U::Pa;    // Pa
            }

            /**
             * @brief Convenience: numeric Kelvin to Tetens pressure (Pa).
             * @param T_numK Numeric Kelvin (interpreted as delta-K for the empirical formula).
             * @return Saturation vapor pressure (Pa).
             * @ingroup Units
             * @since 1.0
             */
            [[nodiscard]] inline auto saturation_vapor_pressure_tetens_K(double T_numK)
            {
                return saturation_vapor_pressure_tetens(thermo::K_delta(T_numK));
            }

            //--------------------------------------------------------------------------
            // Relative humidity -> vapor partial pressure
            //--------------------------------------------------------------------------

            /**
             * @brief Convert relative humidity to vapor partial pressure: e = RH * e_s.
             * @tparam PressureQ pressure quantity (Pa).
             * @param RH   Relative humidity in [0, 1].
             * @param e_s  Saturation vapor pressure (Pa).
             * @return Vapor partial pressure (Pa).
             * @pre 0.0 <= RH <= 1.0.
             * @ingroup Units
             * @since 1.0
             */
            template <class PressureQ>
            [[nodiscard]] constexpr auto vapor_partial_pressure(double RH, const PressureQ& e_s)
                noexcept(noexcept(RH* e_s))
            {
                assert(RH >= 0.0 && RH <= 1.0 && "vapor_partial_pressure: RH must be in [0,1]");
                return RH * e_s; // Pa
            }
       } // inline namespace v1
} // namespace Interstellar::Units

#endif // INTERSTELLAR_UNITS_MATH_HPP
