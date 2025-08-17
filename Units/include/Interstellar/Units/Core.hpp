#ifndef INTERSTELLAR_UNITS_TYPES_HPP
#define INTERSTELLAR_UNITS_TYPES_HPP
#pragma once
/**
 * \file
 * \brief Strongly-typed quantity aliases and a small unit-expression catalog.
 * \defgroup units Types & Units
 * \details
 *   This header defines common quantity type aliases (e.g., \c Length, \c Speed)
 *   and an opt-in catalog of unit expressions under ::Interstellar::Units::U
 *   for readable arithmetic (e.g., <tt>250.0 * U::m_per_s</tt>).
 * \ingroup units
 */

#include "Interstellar/Units/detail/backend.hpp" // must provide detail::sym

namespace Interstellar::Units {
    inline namespace v1 {

        //------------------------------------------------------------------------------
        // Helpers
        //------------------------------------------------------------------------------

        /**
         * \brief Default scalar representation used for quantity aliases.
         * \details Adjust if you need single-precision builds.
         */
        using DefaultRep = double;

        /**
         * \brief Metafunction: quantity type corresponding to a unit expression.
         * \tparam UnitExpr A unit token from ::Interstellar::Units::U or detail::sym.
         * \note Binds the representation to DefaultRep.
         */
        template<auto UnitExpr>
        using QuantityOf = decltype(DefaultRep{ 1.0 } *UnitExpr);

        //------------------------------------------------------------------------------
        // Quantity type aliases (bind to SI canonical units)
        //------------------------------------------------------------------------------

        /// \brief Length quantity (canonical unit: meter).
        /// \ingroup units
        using Length = QuantityOf<detail::sym::m>;

        /// \brief Area quantity (canonical unit: square meter).
        /// \ingroup units
        using Area = QuantityOf<detail::sym::m2>;

        /// \brief Volume quantity (canonical unit: cubic meter).
        /// \ingroup units
        using Volume = QuantityOf<detail::sym::m3>;

        /// \brief Time quantity (canonical unit: second).
        /// \ingroup units
        using Time = QuantityOf<detail::sym::s>;

        /// \brief Mass quantity (canonical unit: kilogram).
        /// \ingroup units
        using Mass = QuantityOf<detail::sym::kg>;

        /// \brief Amount of substance quantity (canonical unit: mole).
        /// \ingroup units
        using AmountOfSubstance = QuantityOf<detail::sym::mol>;

        /// \brief Pressure quantity (canonical unit: pascal).
        /// \ingroup units
        using Pressure = QuantityOf<detail::sym::Pa>;

        /// \brief Speed quantity (canonical unit: meter per second).
        /// \ingroup units
        using Speed = QuantityOf<detail::sym::m / detail::sym::s>;

        /// \brief Acceleration quantity (canonical unit: meter per second squared).
        /// \ingroup units
        using Acceleration = QuantityOf<detail::sym::m / (detail::sym::s * detail::sym::s)>;

        /// \brief Force quantity (canonical unit: newton).
        /// \ingroup units
        using Force = QuantityOf<detail::sym::N>;

        /// \brief Energy quantity (canonical unit: joule).
        /// \ingroup units
        using Energy = QuantityOf<detail::sym::J>;

        /// \brief Power quantity (canonical unit: watt).
        /// \ingroup units
        using Power = QuantityOf<detail::sym::W>;

        //------------------------------------------------------------------------------
        // Unit-expression catalog (opt-in, *no numbers*)
        //------------------------------------------------------------------------------

        /**
         * \brief Unit-expression catalog (opt-in).
         * \details
         *   Qualified access only (e.g., <tt>U::m</tt>, <tt>U::m_per_s</tt>). This avoids
         *   polluting client namespaces with single-letter names, while keeping call sites readable.
         *   Contains only *units* (no numeric quantities).
         * \ingroup units
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
         * \brief Return a quantity as a scalar in the given unit.
         * \tparam Rep   Target scalar representation (defaults to DefaultRep).
         * \tparam Q     A quantity type.
         * \tparam UnitExpr A unit expression (e.g., \c U::m, \c U::m_per_s).
         * \param q      Quantity to convert.
         * \param u      Unit to express \p q in.
         * \return The numeric value of \p q in units of \p u.
         * \pre The expression <tt>q / (Rep{1} * u)</tt> is well-formed.
         * \note This is backend-agnostic: relies only on quantity / (1 * unit).
         * \par Example
         * \code
         * Speed v = 250.0 * U::m_per_s;
         * double v_ms = in(v, U::m_per_s); // 250.0
         * \endcode
         * \ingroup units
         */
        template<class Rep = DefaultRep, class Q, class UnitExpr>
        [[nodiscard]] constexpr Rep in(const Q& q, UnitExpr u)
            noexcept(noexcept(q / (Rep{ 1 } *u)))
            requires requires { q / (Rep{ 1 } *u); }  // participates only when valid
        {
            // Dividing a quantity by (1 * unit) yields a dimensionless number.
            // We cast to the requested representation explicitly.
            return static_cast<Rep>(q / (Rep{ 1 } *u));
        }

        //------------------------------------------------------------------------------
        // Backwards-compatibility aliases (optional; remove after migration)
        //------------------------------------------------------------------------------
        [[deprecated("Use AmountOfSubstance")]]
        using Amount = AmountOfSubstance;

    } // inline namespace v1
} // namespace Interstellar::Units

#endif // INTERSTELLAR_UNITS_TYPES_HPP
