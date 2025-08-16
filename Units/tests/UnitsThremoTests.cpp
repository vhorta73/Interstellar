// Units/tests/interstellar/units/UnitsThermoTests.cpp
#include <gtest/gtest.h>

#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Math.hpp"
#include "helpers/UnitsTestHelpers.hpp"

using namespace Interstellar::Units;
using namespace Interstellar::Units::thermo;    // K_delta, as_deltaK
using namespace Interstellar::Units::length;    // cubic_meters
using namespace Interstellar::Units::literals;  // 1.0_mol

namespace UnitsThermoTests {

    TEST(UnitsThermoTests, ideal_gas) {
        const auto n = 1.0_mol;
        const auto temp_dK = K_delta(273.15);        // 273.15 K (delta-K)
        const auto V = cubic_meters(22.414e-3);

        const auto P = ideal_gas_pressure(n, temp_dK, V);
        EXPECT_NEAR_Q(P, U::Pa, 101325.0, 0.01);     // +/-1%
    }

    TEST(UnitsThermoTests, tetens_saturation_vapor_pressure_at_20C_is_2338_Pa) {
        const auto temp_dK = K_delta(293.15);        // 20 C
        const auto es = saturation_vapor_pressure_tetens(temp_dK);
        EXPECT_NEAR_Q(es, U::Pa, 2338.0, 0.03);
    }

    TEST(UnitsThermoTests, relative_humidity_to_partial_pressure) {
        const auto temp_dK = K_delta(298.15);        // 25 C
        const auto es = saturation_vapor_pressure_tetens(temp_dK);   // Pa
        const auto e = vapor_partial_pressure(0.5, es);             // 50% RH

        // Expected partial pressure is simply 0.5 * es; compare as a scalar in Pa via helper.
        EXPECT_NEAR_Q(e, U::Pa, 0.5 * static_cast<double>(es / (1.0 * U::Pa)), 1e-12);
    }

} // namespace UnitsThermoTests
