// Units/tests/interstellar/units/UnitsFluidsTests.cpp
#include <gtest/gtest.h>

#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Math.hpp"
#include "Interstellar/Units/Constants.hpp"
#include "helpers/UnitsTestHelpers.hpp"   // EXPECT_NEAR_Q

using namespace Interstellar::Units;
using namespace Interstellar::Units::length;
using namespace Interstellar::Units::time;
using namespace Interstellar::Units::mass;
using namespace Interstellar::Units::mech;

namespace UnitsFluidsTests {

    // Hydrostatic pressure head: delta-P = rho g h
    TEST(UnitsFluidsTests, hydrostatic_pressure_head) {
        // Fresh water 1000 kg/m^3
        const auto rho = kilograms(1000.0) / cubic_meters(1.0); // Density
        const auto h = meters(10.0);

        const auto dP = hydrostatic_pressure(rho, g0, h);
        // Expect 98.0665 kPa at 10 m depth (ignoring atmosphere)
        EXPECT_NEAR_Q(dP, U::Pa, 98066.5, 1e-3);
    }

    // Reynolds number: Re = (rho v L) / mu  (dimensionless)
    TEST(UnitsFluidsTests, reynolds_number_dimensionless) {
        // Air at sea level (approx)
        const auto rho = kilograms(1.225) / cubic_meters(1.0);   // kg/m^3
        const auto v = meters_per_second(10.0);                // m/s
        const auto L = meters(0.1);                            // m
        const auto mu = pascals(1.8e-5) * U::s;                 // dynamic viscosity (Pas)

        const auto Re = (rho * v * L) / mu;                     // dimensionless

        // Compare as a pure number (unit 'one'); keep everything in quantities.
        constexpr double expected = (1.225 * 10.0 * 0.1) / (1.8e-5); // 68055.6
        EXPECT_NEAR_Q(Re, Interstellar::Units::detail::mu::one, expected, expected * 1e-9);
    }

} // namespace UnitsFluidsTests
