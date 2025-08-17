// Units/tests/interstellar/units/UnitsThermoTests.cpp
#include <gtest/gtest.h>

#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Math.hpp"
#include "helpers/UnitsTestHelpers.hpp" // EXPECT_NEAR_Q_ABS / EXPECT_NEAR_Q_REL

namespace interstellar::test::units {

    using namespace Interstellar::Units;
    using namespace Interstellar::Units::thermo;    // K_delta, as_deltaK
    using namespace Interstellar::Units::length;    // cubic_meters
    using namespace Interstellar::Units::literals;  // 1.0_mol

    TEST(UnitsThermo, ideal_gas) {
        const auto n = 1.0_mol;
        const auto temp_dK = K_delta(273.15);            // 273.15 K (delta-K)
        const auto V = cubic_meters(22.414e-3);    // 22.414 L

        const auto P = ideal_gas_pressure(n, temp_dK, V);
        SCOPED_TRACE(::testing::Message() << "P = " << P);

        // Relative tolerance: +/-1%
        EXPECT_NEAR_Q_REL(P, U::Pa, 101325.0, 0.01);
    }

    TEST(UnitsThermo, tetens_saturation_vapor_pressure_at_20C_is_2338_Pa) {
        const auto temp_dK = K_delta(293.15);  // 20 C
        const auto es = saturation_vapor_pressure_tetens(temp_dK);

        SCOPED_TRACE(::testing::Message() << "es(20C) = " << es);

        // Relative tolerance: +/-3%
        EXPECT_NEAR_Q_REL(es, U::Pa, 2338.0, 0.03);
    }

    TEST(UnitsThermo, relative_humidity_to_partial_pressure) {
        const auto temp_dK = K_delta(298.15);               // 25 C
        const auto es = saturation_vapor_pressure_tetens(temp_dK); // Pa
        const auto e = vapor_partial_pressure(0.5, es);           // 50% RH

        SCOPED_TRACE(::testing::Message() << "es(25C) = " << es << ", e(50% RH) = " << e);

        // Compare against 0.5 * es: extract expected as a scalar in Pa; keep LHS a quantity.
        const double es_Pa = to_number(es, U::Pa); // to_number lives in interstellar::test::units
        EXPECT_NEAR_Q_REL(e, U::Pa, 0.5 * es_Pa, 1e-12);
    }
}