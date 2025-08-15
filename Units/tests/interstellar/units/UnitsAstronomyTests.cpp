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

namespace interstellar_units {

    using namespace Interstellar::Units;
    using namespace Interstellar::Units::sym;

    TEST(astronomy, g0_greater_than_one_mps2) {
        constexpr auto one_m_per_s2 = 1.0 * m / (s * s);
        EXPECT_GT(g0, one_m_per_s2);
    }

    TEST(astronomy, earth_escape_velocity_near_surface_approx_11186_m_s) {
        const auto ve = escape_velocity(GM_Earth, EarthRadius); // quantity (m/s)
        // Four args: (q, unit, expected, rel_tol)
        EXPECT_NEAR_Q(ve, m / s, 11186.0, 0.02);
    }

}