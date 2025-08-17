// Units/tests/interstellar/units/UnitsAstronomyTests.cpp
#include <gtest/gtest.h>

#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Constants.hpp"
#include "Interstellar/Units/Math.hpp"
#include "helpers/UnitsTestHelpers.hpp"

/**
 * @file UnitsAstronomyTests.cpp
 * @brief Basic orbital/planetary physics checks relevant to gameplay.
 * @ingroup UnitsTests
 */

using namespace Interstellar::Units;

namespace UnitsAstronomyTests {

    TEST(UnitsAstronomyTests, g0_greater_than_one_mps2) {
        // Compare using quantities to keep dimensional safety.
        const auto one_m_per_s2 = mech::meters_per_second2(1.0);
        EXPECT_GT(kStandardGravity, one_m_per_s2);
    }

    TEST(UnitsAstronomyTests, earth_escape_velocity_near_surface_approx_11186_m_s) {
        const auto ve = escape_velocity(kEarthMu, kEarthRadius);   // quantity (m/s)
        // Four args: (q, unit, expected, rel_tol)
        EXPECT_NEAR_Q(ve, U::m_per_s, 11186.0, 0.02);             // +/-2% is fine for coarse check
    }

} // namespace UnitsAstronomyTests
