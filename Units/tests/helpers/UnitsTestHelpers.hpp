#pragma once
#include <gtest/gtest.h>
#include <mp-units/ostream.h>   // test-only pretty printing of quantities
#include <mp-units/math.h>      // ::mp_units::abs

namespace interstellar::test::units {

    // Compile-time guard: participate only if the unit/quantity math we need is valid.
    template<class Q, class U>
    concept QuantityComparableWithUnit =
        requires (const Q & q, double x, const U & u) {
            { x* u };                         // scalar * unit -> quantity (build expected_q)
            { q - (x * u) };                   // same-dimension subtraction is valid
            { (q - (x * u))* (q - (x * u)) }; // we can square the difference (for tol^2 compare)
    };

    // Absolute tolerance: |q - expected*unit| <= abs_tol*unit
    template<class Q, class U>
    inline void expect_near_q_abs(const Q& q, const U& unit,
        double expected, double abs_tol)
        requires QuantityComparableWithUnit<Q, U>
    {
        ASSERT_GE(abs_tol, 0.0) << "abs_tol must be >= 0";

        const auto expected_q = expected * unit;
        const auto tol_q = abs_tol * unit;

        const auto err2 = (q - expected_q) * (q - expected_q);
        const auto tol2 = tol_q * tol_q;

        EXPECT_LE(err2, tol2)
            << "expected " << expected_q << " (+/-" << tol_q << "), got " << q;
    }

    // Relative tolerance with floor:
    // |q - expected*unit| <= max(rel_tol * |expected*unit|, abs_floor*unit)
    template<class Q, class U>
    inline void expect_near_q_rel(const Q& q, const U& unit,
        double expected, double rel_tol, double abs_floor = 0.0)
        requires QuantityComparableWithUnit<Q, U>
    {
        ASSERT_GE(rel_tol, 0.0) << "rel_tol must be >= 0";
        ASSERT_GE(abs_floor, 0.0) << "abs_floor must be >= 0";

        const auto expected_q = expected * unit;
        const auto rel_tol_q = rel_tol * ::mp_units::abs(expected_q); // quantity tolerance
        const auto tol_q = (rel_tol_q > abs_floor * unit) ? rel_tol_q
            : abs_floor * unit;

        const auto err2 = (q - expected_q) * (q - expected_q);
        const auto tol2 = tol_q * tol_q;

        EXPECT_LE(err2, tol2)
            << "expected " << expected_q << " (+/-" << tol_q << "), got " << q;
    }

    /**
     * @brief Extract a scalar value of a quantity expressed in a given unit.
     * @tparam Rep numeric type to return (defaults to double)
     *
     * Example:
     *   double p = to_number(101325.0 * U::Pa, U::Pa); // -> 101325.0
     */
    template<class Rep = double, class Q, class U>
    inline Rep to_number(const Q& q, const U& unit)
        noexcept(noexcept(q / unit))
        requires requires (const Q& qq, const U& uu) {
        static_cast<Rep>(qq / uu);
    }
    {
        return static_cast<Rep>(q / unit);
    }

} // namespace interstellar::test::units

// Macros for convenience
#define EXPECT_NEAR_Q_ABS(q, unit, expected, abs_tol) \
  ::interstellar::test::units::expect_near_q_abs((q), (unit), (expected), (abs_tol))

#define EXPECT_NEAR_Q_REL(q, unit, expected, rel_tol) \
  ::interstellar::test::units::expect_near_q_rel((q), (unit), (expected), (rel_tol))

// With floor for near-zero expected values:
#define EXPECT_NEAR_Q_REL_FLOOR(q, unit, expected, rel_tol, abs_floor) \
  ::interstellar::test::units::expect_near_q_rel((q), (unit), (expected), (rel_tol), (abs_floor))
