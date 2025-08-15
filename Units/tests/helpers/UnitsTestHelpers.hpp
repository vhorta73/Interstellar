#pragma once
#include <gtest/gtest.h>
#include <mp-units/ostream.h>   // <-- add this for operator<< on quantities
#include <mp-units/systems/si.h>
#include <cmath>

namespace interstellar::units {

    template<class Q, class U>
    inline void expect_near_q(const Q& q, const U& unit,
        double expected, double rel_tol)
    {
        const auto expected_q = expected * unit;
        const auto tol_q = rel_tol * expected_q;

        const auto err2 = (q - expected_q) * (q - expected_q);
        const auto tol2 = tol_q * tol_q;

        EXPECT_LE(err2, tol2) << "expected " << expected_q
            << " (+/-" << tol_q << "), got " << q;
    }

} // namespace interstellar::units

#define EXPECT_NEAR_Q(q, unit, expected, rel_tol) \
  ::interstellar::units::expect_near_q((q), (unit), (expected), (rel_tol))
