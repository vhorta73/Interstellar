// Units/tests/interstellar/units/UnitsThermoTetensMore.cpp
#include <gtest/gtest.h>
#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Math.hpp"
#include "helpers/UnitsTestHelpers.hpp"

namespace interstellar::test::units {
    using namespace Interstellar::Units;

    TEST(UnitsThermo, tetens_at_0C_is_611_Pa_approx) {
        const auto es0 = saturation_vapor_pressure_tetens(thermo::K_delta(273.15));
        EXPECT_NEAR_Q_REL(es0, U::Pa, 611.0, 0.05); // ~5% tolerance
    }

    TEST(UnitsThermo, tetens_monotonic_increasing_with_temperature) {
        const auto es10 = saturation_vapor_pressure_tetens(thermo::K_delta(273.15 + 10.0));
        const auto es20 = saturation_vapor_pressure_tetens(thermo::K_delta(273.15 + 20.0));
        EXPECT_LT(es10, es20);
    }
}
