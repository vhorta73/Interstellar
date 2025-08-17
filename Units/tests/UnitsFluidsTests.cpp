// Units/tests/interstellar/units/UnitsFluidsTests.cpp
#include <gtest/gtest.h>

#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Math.hpp"
#include "Interstellar/Units/Constants.hpp"
#include "helpers/UnitsTestHelpers.hpp"   // EXPECT_NEAR_Q_ABS / EXPECT_NEAR_Q_REL / EXPECT_NEAR_Q_REL_FLOOR

namespace interstellar::test::units {

    // Bring in only what you need, scoped to the test namespace
    using namespace Interstellar::Units;
    using namespace Interstellar::Units::length;
    using namespace Interstellar::Units::time;
    using namespace Interstellar::Units::mass;
    using namespace Interstellar::Units::mech;

    // Hydrostatic pressure head: delta-P = rho g h
    TEST(UnitsFluids, hydrostatic_pressure_head) {
        // Fresh water 1000 kg/m^3
        const auto rho = kilograms(1000.0) / cubic_meters(1.0); // Density
        const auto h = meters(10.0);

        const auto dP = hydrostatic_pressure(rho, kStandardGravity, h);
        // Expect 98.0665 kPa at 10 m depth (ignoring atmosphere)
        EXPECT_NEAR_Q_ABS(dP, U::Pa, 98066.5, 1e-3);
    }

    // Reynolds number: Re = (rho v L) / mu  (dimensionless)
    TEST(UnitsFluids, reynolds_number_dimensionless) {
        // Air at sea level (approx)
        const auto rho = kilograms(1.225) / cubic_meters(1.0); // kg/m^3
        const auto v = meters_per_second(10.0);                // m/s
        const auto L = meters(0.1);                            // m
        const auto mu = pascals(1.8e-5) * U::s;                // dynamic viscosity (Pa*s)

        const auto Re = (rho * v * L) / mu;                    // dimensionless

        // Compare as a pure number in the 'one' unit (dimensionless), still using quantities.
        constexpr double expected = (1.225 * 10.0 * 0.1) / (1.8e-5); // 68055.555...
        EXPECT_NEAR_Q_ABS(Re, U::one, expected, expected * 1e-9);
    }
}
