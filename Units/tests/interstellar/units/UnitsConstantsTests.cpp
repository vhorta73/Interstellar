// Units/tests/interstellar/units/UnitsConstantsTests.cpp
#include <gtest/gtest.h>
#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Constants.hpp"
#include "helpers/UnitsTestHelpers.hpp"

namespace interstellar::test::units {
    using namespace Interstellar::Units;

    TEST(UnitsConstants, standard_pressure_value_and_unit) {
        EXPECT_NEAR_Q_ABS(kStandardPressure, U::Pa, 101325.0, 1e-9);
    }

    TEST(UnitsConstants, stefan_boltzmann_units_and_value) {
        // Units: W/(m^2*K^4)
        const auto one = (1.0 * U::W) / (1.0 * U::m2 * U::K * U::K * U::K * U::K);
        EXPECT_NEAR_Q_REL(kStefanBoltzmann / one, U::one, 5.670374419e-8, 1e-12);
    }

    TEST(UnitsConstants, gas_constant_units_and_value) {
        const auto one = (1.0 * U::J) / (1.0 * U::mol * U::K);
        EXPECT_NEAR_Q_REL(kGasConstantR / one, U::one, 8.31446261815324, 1e-15);
    }

    TEST(UnitsConstants, earth_radius_and_mu_units_and_values) {
        EXPECT_NEAR_Q_ABS(kEarthRadius, U::m, 6'371'008.8, 1e-3);
        const auto mu_unit = U::m3 / U::s2;
        EXPECT_NEAR_Q_REL(kEarthMu / mu_unit, U::one, 3.986004418e14, 1e-15);
    }
}
