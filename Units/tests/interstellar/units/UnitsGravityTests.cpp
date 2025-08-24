// Units/tests/interstellar/units/UnitsGravityTests.cpp
#include <gtest/gtest.h>

#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Constants.hpp"
#include "helpers/UnitsTestHelpers.hpp"  // EXPECT_NEAR_Q_ABS / EXPECT_NEAR_Q_REL

namespace interstellar::test::units {

    using namespace Interstellar::Units;
    using namespace Interstellar::Units::mass;

    TEST(UnitsGravity, newtons_second_law) {
        const auto m = kilograms(80.0);
        const auto F = m * kStandardGravity;  // Force (N)

        SCOPED_TRACE(::testing::Message() << "F = " << F);

        // Expected magnitude: 80 kg * 9.80665 m/s^2 = 784.532 N
        EXPECT_NEAR_Q_ABS(F, U::N, 80.0 * 9.80665, 1e-9);
    }

}
