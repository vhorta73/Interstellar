#include <gtest/gtest.h>
#include "Interstellar/Units/Constants.hpp"
#include "helpers/UnitsTestHelpers.hpp"

using namespace Interstellar::Units;
using namespace Interstellar::Units::sym;

TEST(UnitsConstants, g0_within_expected_range) {
    EXPECT_NEAR_Q(g0, m / (s * s), 9.80665, 1e-8);
}

TEST(UnitsConstants, standard_atmosphere) {
    EXPECT_NEAR_Q(p0, Pa, 101325.0, 1e-10);
}

TEST(UnitsConstants, universal_gas_constant_R) {
    EXPECT_NEAR_Q(R, J / (mol * K), 8.31446261815324, 1e-12);
}

TEST(UnitsConstants, Earth_parameters_sane) {
    EXPECT_NEAR_Q(EarthRadius, m, 6'371'008.8, 1e-8);
    EXPECT_NEAR_Q(EarthMass, kg, 5.9722e24, 1e-12);
}
