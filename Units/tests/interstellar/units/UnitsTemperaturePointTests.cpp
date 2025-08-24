// Units/tests/interstellar/units/UnitsTemperaturePointTests.cpp
#include <gtest/gtest.h>
#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Math.hpp"
#include "Interstellar/Units/Constants.hpp"
#include "helpers/UnitsTestHelpers.hpp"

namespace interstellar::test::units {
    using namespace Interstellar::Units;

    TEST(UnitsThermo, equilibrium_temp_point_matches_delta_plus_absolute_zero) {
        const auto Td = equilibrium_temp(kSolarConstant, 0.3); // delta-K
        const auto Tp = equilibrium_temp_point(kSolarConstant, 0.3); // point

        // Tp - absolute_zero == Td
        const auto Td_from_point = Tp - detail::si::absolute_zero;
        EXPECT_NEAR_Q_REL(Td_from_point, thermo::K_delta(1.0), Interstellar::Units::in(Td, thermo::K_delta(1.0)), 1e-12);
    }
}
