// Units/tests/interstellar/units/UnitsRadiationEdgeTests.cpp
#include <gtest/gtest.h>
#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Math.hpp"
#include "Interstellar/Units/Constants.hpp"
#include "helpers/UnitsTestHelpers.hpp"

namespace interstellar::test::units {
    using namespace Interstellar::Units;

    TEST(UnitsRadiation, equilibrium_temp_albedo_one_is_zero) {
        const auto T = equilibrium_temp(kSolarConstant, 1.0);
        EXPECT_NEAR_Q_ABS(T, thermo::K_delta(1.0), 0.0, 0.0);
    }
}
