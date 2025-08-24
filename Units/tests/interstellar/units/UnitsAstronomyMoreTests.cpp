// Units/tests/interstellar/units/UnitsAstronomyMoreTests.cpp
#include <gtest/gtest.h>
#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Constants.hpp"
#include "Interstellar/Units/Math.hpp"
#include "helpers/UnitsTestHelpers.hpp"

namespace interstellar::test::units {
    using namespace Interstellar::Units;

    TEST(UnitsAstronomy, escape_velocity_scales_with_inverse_sqrt_radius) {
        const auto ve1 = escape_velocity(kEarthMu, kEarthRadius);
        const auto ve2 = escape_velocity(kEarthMu, 2.0 * kEarthRadius);
        // v2 should be v1 / sqrt(2)
        EXPECT_NEAR_Q_REL(ve2, U::m_per_s, Interstellar::Units::in(ve1, U::m_per_s) / std::sqrt(2.0), 1e-12);
    }
}
