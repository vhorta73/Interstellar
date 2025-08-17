#pragma once
#ifndef INTERSTELLAR_UNITS_MATH_HPP
#define INTERSTELLAR_UNITS_MATH_HPP

#include <cmath>
#include <cassert>

/**
 * \file
 * \brief Small, unit-safe helpers used by gameplay systems.
 * \details
 *   All temperature math is performed on \b delta-K (temperature \em differences).
 *   Callers may pass absolute K (quantity_point) or delta K (quantity);
 *   we normalize to delta K internally via \c thermo::as_deltaK .
 *
 * \par External expectations
 *   This header expects the following helpers to exist:
 *   - \c thermo::as_deltaK(T): converts absolute or delta K to a delta-K quantity.
 *   - \c thermo::K_delta(x): constructs a delta-K quantity from a numeric scalar.
 *
 * \ingroup units
 */

#include "Interstellar/Units/Units.hpp"        // aliases + U:: catalog
#include "Interstellar/Units/Constants.hpp"    // kGasConstantR, kStefanBoltzmann, etc.
#include <mp-units/math.h>                     // sqrt for quantities

namespace Interstellar::Units {
    inline namespace v1 {

        //------------------------------------------------------------------------------
        // Ideal gas law: P = n * R * T / V
        //------------------------------------------------------------------------------

        /**
         * \brief Ideal gas law: \f$P = \frac{n \, R \, T}{V}\f$.
         * \tparam AmountQ   quantity type compatible with amount of substance (mol).
         * \tparam TempQ     absolute K (quantity_point) or delta-K (quantity).
         * \tparam VolQ      quantity type compatible with volume (m^3).
         * \param n          Amount of substance (mol).
         * \param tempK_any  Absolute or delta Kelvin.
         * \param vol        Volume (m^3).
         * \return Pressure (Pa).
         * \pre \c vol > 0.
         * \note Uses \c kGasConstantR (CODATA 2018). Temperature is treated as delta-K.
         * \ingroup units
         * \par Example
         * \code
         * using namespace Interstellar::Units;
         * auto p = ideal_gas_pressure(1.0 * U::mol, 300.0 * U::K, 0.024 * U::m3);
         * \endcode
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
            // Preconditions (debug assertion; document as contract)
            assert(in(vol, U::m3) > 0.0 && "ideal_gas_pressure: volume must be > 0");

            const auto dT = thermo::as_deltaK(tempK_any); // delta-K
            return (n * kGasConstantR * dT) / vol;        // Pa
        }

        //------------------------------------------------------------------------------
        // Hydrostatic head: delta-P = p * g * h
        //------------------------------------------------------------------------------

        /**
         * \brief Hydrostatic pressure head: \f$\Delta p = \rho \, g \, h\f$.
         * \tparam DensityQ   density quantity (kg/m^3).
         * \tparam AccelQ     acceleration (m/s^2).
         * \tparam LengthQ    depth/height (m).
         * \return Pressure (Pa).
         * \pre \c depth >= 0.
         * \ingroup units
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

        //------------------------------------------------------------------------------
        // Escape velocity: v_e = sqrt(2 mu / r)
        //------------------------------------------------------------------------------

        /**
         * \brief Escape velocity: \f$v_e = \sqrt{\frac{2\,\mu}{r}}\f$, with \f$\mu = G M\f$.
         * \tparam MuQ     standard gravitational parameter (m^3/s^2).
         * \tparam LengthQ radius (m).
         * \return Speed (m/s).
         * \pre \c radius > 0.
         * \ingroup units
         */
        template <class MuQ, class LengthQ>
        [[nodiscard]] constexpr auto escape_velocity(const MuQ& mu, const LengthQ& radius)
            noexcept(noexcept(detail::mu::sqrt(2.0 * mu / radius)))
            requires requires { detail::mu::sqrt(2.0 * mu / radius); }
        {
            assert(in(radius, U::m) > 0.0 && "escape_velocity: radius must be > 0");
            return detail::mu::sqrt(2.0 * mu / radius);
        }

        //------------------------------------------------------------------------------
        // No-greenhouse radiative equilibrium
        // T = ((1 - A) * S / (4 * sigma))^(1/4)
        //------------------------------------------------------------------------------

        /**
         * \brief No-greenhouse radiative equilibrium temperature (delta-K form).
         * \details \f$T = \left(\frac{(1 - A)\,S}{4\sigma}\right)^{1/4}\f$.
         * \param S      Stellar flux (irradiance) in W/m^2.
         * \param albedo Bond albedo in [0, 1].
         * \return Temperature \em difference (delta-K).
         * \pre \c 0.0 <= albedo <= 1.0.
         * \note Returns a \b delta-K quantity per project convention. For an absolute
         *       temperature point, use \ref equilibrium_temp_point.
         * \ingroup units
         */
        template <class FluxQ>
        [[nodiscard]] constexpr auto equilibrium_temp(const FluxQ& S, double albedo)
            noexcept(noexcept(detail::mu::sqrt(detail::mu::sqrt(((1.0 - albedo)* S) / (4.0 * kStefanBoltzmann)))))
            requires requires { ((1.0 - albedo)* S) / (4.0 * kStefanBoltzmann); }
        {
            assert(albedo >= 0.0 && albedo <= 1.0 && "equilibrium_temp: albedo must be in [0,1]");
            const auto x = ((1.0 - albedo) * S) / (4.0 * kStefanBoltzmann);
            // mp-units: prefer sqrt(sqrt(x)) over pow(x, 0.25)
            return detail::mu::sqrt(::mp_units::sqrt(x));
        }

        /**
         * \brief Absolute (Kelvin) equilibrium temperature as a quantity \b point.
         * \details Uses the same expression as \ref equilibrium_temp and returns K as a quantity_point.
         * \ingroup units
         */
        template <class FluxQ, class KelvinPoint = decltype(0.0 * U::K + 0.0 * U::K)> // requires a K point type from your backend
        [[nodiscard]] constexpr auto equilibrium_temp_point(const FluxQ& S, double albedo)
            noexcept(noexcept(KelvinPoint{ equilibrium_temp(S, albedo) }))
        {
            // Construct a temperature point at the computed delta-K above absolute zero.
            return KelvinPoint{ equilibrium_temp(S, albedo) };
        }

        //------------------------------------------------------------------------------
        // Tetens saturation vapor pressure
        // e_s = C0 * exp( A * Tc / (Tc + B) ), with Tc in C (delta-K numerically)
        //------------------------------------------------------------------------------

        /**
         * \brief Tetens saturation vapor pressure (Pa).
         * \details \f$ e_s = C_0 \exp\!\left(\frac{A\,T_c}{T_c + B}\right) \f$.\n
         *   Uses \c Tc in C; implemented via delta-K with the usual numeric equivalence.
         * \param tempK_any Absolute or delta Kelvin.
         * \return Saturation vapor pressure (Pa).
         * \pre Valid temperature such that \c (Tc + B) != 0.
         * \ingroup units
         */
        template<class TempQ>
        [[nodiscard]] inline auto saturation_vapor_pressure_tetens(const TempQ& tempK_any)
            noexcept(noexcept(thermo::as_deltaK(tempK_any)))
            requires requires { thermo::as_deltaK(tempK_any); }
        {
            const auto dT = thermo::as_deltaK(tempK_any);                  // delta-K
            const auto Tc = dT - thermo::K_delta(273.15);                  // "C" as delta-K
            const auto tetens_B = thermo::K_delta(237.3);                  // delta-K

            constexpr double tetens_A = 17.27;                            // dimensionless
            constexpr double tetens_C0 = 610.78;                           // Pa (scalar)

            const double frac = static_cast<double>(Tc / (Tc + tetens_B)); // dimensionless
            const double expo = tetens_A * frac;

            return (tetens_C0 * std::exp(expo)) * U::Pa;                   // Pa
        }

        /**
         * \brief Convenience: numeric Kelvin to Tetens pressure (Pa).
         * \param T_numK Numeric Kelvin (interpreted as \b delta-K for the empirical formula).
         * \ingroup units
         */
        [[nodiscard]] inline auto saturation_vapor_pressure_tetens_K(double T_numK)
        {
            return saturation_vapor_pressure_tetens(thermo::K_delta(T_numK));
        }

        //------------------------------------------------------------------------------
        // Relative humidity -> vapor partial pressure
        //------------------------------------------------------------------------------

        /**
         * \brief Convert relative humidity to vapor partial pressure: \f$e = \mathrm{RH}\, e_s\f$.
         * \param RH   Relative humidity in [0, 1].
         * \param e_s  Saturation vapor pressure (Pa).
         * \return Vapor partial pressure (Pa).
         * \pre \c 0.0 <= RH <= 1.0.
         * \ingroup units
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
