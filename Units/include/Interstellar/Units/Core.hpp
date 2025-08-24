#ifndef INTERSTELLAR_UNITS_TYPES_HPP
#define INTERSTELLAR_UNITS_TYPES_HPP
#pragma once
/**
 * @file
 * @brief Strongly-typed quantity aliases and a small unit-expression catalog.
 * @details
 *   This header defines common quantity type aliases (e.g., Length, Speed)
 *   and an opt-in catalog of unit expressions under ::Interstellar::Units::U
 *   for readable arithmetic (e.g., 250.0 * U::m_per_s).
 */

#include "Interstellar/Units/detail/backend.hpp" // must provide detail::sym

namespace Interstellar::Units {
    inline namespace v1 {

        //------------------------------------------------------------------------------
        // Helpers
        //------------------------------------------------------------------------------

        /**
         * @brief Default scalar representation used for quantity aliases.
         * @details Adjust if you need single-precision builds.
         * @ingroup Units
         * @since 1.0
         */
        using DefaultRep = double;

        /**
         * @brief Metafunction: quantity type corresponding to a unit expression.
         * @tparam UnitExpr A unit token from ::Interstellar::Units::U or detail::sym.
         * @note Binds the representation to DefaultRep.
         * @ingroup Units
         * @since 1.0
         */
        template<auto UnitExpr>
        using QuantityOf = decltype(DefaultRep{ 1.0 } *UnitExpr);

        //------------------------------------------------------------------------------
        // Quantity type aliases (bind to SI canonical units)
        //------------------------------------------------------------------------------

        /** @brief Length quantity (canonical unit: meter). @ingroup Units @since 1.0 */
        using Length = QuantityOf<detail::sym::m>;

        /** @brief Area quantity (canonical unit: square meter). @ingroup Units @since 1.0 */
        using Area = QuantityOf<detail::sym::m2>;

        /** @brief Volume quantity (canonical unit: cubic meter). @ingroup Units @since 1.0 */
        using Volume = QuantityOf<detail::sym::m3>;

        /** @brief Time quantity (canonical unit: second). @ingroup Units @since 1.0 */
        using Time = QuantityOf<detail::sym::s>;

        /** @brief Mass quantity (canonical unit: kilogram). @ingroup Units @since 1.0 */
        using Mass = QuantityOf<detail::sym::kg>;

        /** @brief Amount of substance quantity (canonical unit: mole). @ingroup Units @since 1.0 */
        using AmountOfSubstance = QuantityOf<detail::sym::mol>;

        /** @brief Pressure quantity (canonical unit: pascal). @ingroup Units @since 1.0 */
        using Pressure = QuantityOf<detail::sym::Pa>;

        /** @brief Speed quantity (canonical unit: meter per second). @ingroup Units @since 1.0 */
        using Speed = QuantityOf<detail::sym::m / detail::sym::s>;

        /** @brief Acceleration quantity (canonical unit: meter per second squared). @ingroup Units @since 1.0 */
        using Acceleration = QuantityOf<detail::sym::m / (detail::sym::s * detail::sym::s)>;

        /** @brief Force quantity (canonical unit: newton). @ingroup Units @since 1.0 */
        using Force = QuantityOf<detail::sym::N>;

        /** @brief Energy quantity (canonical unit: joule). @ingroup Units @since 1.0 */
        using Energy = QuantityOf<detail::sym::J>;

        /** @brief Power quantity (canonical unit: watt). @ingroup Units @since 1.0 */
        using Power = QuantityOf<detail::sym::W>;

        //------------------------------------------------------------------------------
        // Unit-expression catalog (opt-in, no numbers)
        //------------------------------------------------------------------------------

        /**
         * @brief Unit-expression catalog (opt-in).
         * @details
         *   Qualified access only (e.g., U::m, U::m_per_s). This avoids
         *   polluting client namespaces with short names while keeping call sites readable.
         *   Contains only units (no numeric quantities).
         * @ingroup Units
         * @since 1.0
         */
        namespace U {
            inline constexpr auto m = detail::sym::m;
            inline constexpr auto m2 = detail::sym::m2;
            inline constexpr auto m3 = detail::sym::m3;
            inline constexpr auto s = detail::sym::s;
            inline constexpr auto kg = detail::sym::kg;
            inline constexpr auto mol = detail::sym::mol;
            inline constexpr auto Pa = detail::sym::Pa;
            inline constexpr auto N = detail::sym::N;
            inline constexpr auto J = detail::sym::J;
            inline constexpr auto W = detail::sym::W;

            inline constexpr auto m_per_s = detail::sym::m / detail::sym::s;
            inline constexpr auto m_per_s2 = detail::sym::m / (detail::sym::s * detail::sym::s);
        } // namespace U

        //------------------------------------------------------------------------------
        // Scalar extraction utility
        //------------------------------------------------------------------------------

        /**
         * @brief Return a quantity as a scalar in the given unit.
         * @tparam Rep Target scalar representation (defaults to DefaultRep).
         * @tparam Q A quantity type.
         * @tparam UnitExpr A unit expression (e.g., U::m, U::m_per_s).
         * @param q Quantity to convert.
         * @param u Unit to express q in.
         * @return The numeric value of q in units of u.
         * @pre The expression q / (Rep{1} * u) is well-formed.
         * @note Backend-agnostic: relies only on quantity / (1 * unit).
         * @par Example
         * @code
         * Speed v = 250.0 * U::m_per_s;
         * double v_ms = in(v, U::m_per_s); // 250.0
         * @endcode
         * @ingroup Units
         * @since 1.0
         */
        template<class Rep = DefaultRep, class Q, class UnitExpr>
        [[nodiscard]] constexpr Rep in(const Q& q, UnitExpr u)
            noexcept(noexcept(q / (Rep{ 1 } *u)))
            requires requires { q / (Rep{ 1 } *u); }
        {
            return static_cast<Rep>(q / (Rep{ 1 } *u));
        }

    } // inline namespace v1
} // namespace Interstellar::Units

#endif // INTERSTELLAR_UNITS_TYPES_HPP
