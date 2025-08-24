// Units/tests/interstellar/units/UnitsFluidsZeroDepthTests.cpp
#include <gtest/gtest.h>
#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Math.hpp"
#include "helpers/UnitsTestHelpers.hpp"

namespace interstellar::test::units {
    using namespace Interstellar::Units;
    using namespace Interstellar::Units::mass;
    using namespace Interstellar::Units::length;

    TEST(UnitsFluids, hydrostatic_zero_depth_is_zero_pressure) {
        const auto rho = kilograms(1000.0) / cubic_meters(1.0);
        const auto dP = hydrostatic_pressure(rho, kStandardGravity, 0.0 * U::m);
        EXPECT_NEAR_Q_ABS(dP, U::Pa, 0.0, 0.0);
    }
}
