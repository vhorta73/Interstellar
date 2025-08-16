// Units/tests/interstellar/units/UnitsFacadeTests.cpp
#include <gtest/gtest.h>

#include "Interstellar/Units/Units.hpp"
#include "helpers/UnitsTestHelpers.hpp"  // EXPECT_NEAR_Q

using namespace Interstellar::Units;
using namespace Interstellar::Units::length;
using namespace Interstellar::Units::time;
using namespace Interstellar::Units::mass;
using namespace Interstellar::Units::mech;
using namespace Interstellar::Units::thermo;
using namespace Interstellar::Units::qmath;
using namespace Interstellar::Units::literals;

namespace UnitsFacadeTests {

    TEST(Units_Facade, types_exist_and_basic_construction) {
        const Length  L = meters(2.0);
        const Area    A = square_meters(3.0);
        const Volume  V = cubic_meters(4.0);
        const Time    T = seconds(5.0);
        const Mass    M = kilograms(6.0);
        const Amount  N = 1.0_mol;

        (void)L; (void)A; (void)V; (void)T; (void)M; (void)N;
        SUCCEED();
    }

    TEST(Units_Facade, U_catalog_base_and_derived) {
        // Base
        EXPECT_NEAR_Q(1.0 * U::m, U::m, 1.0, 0.0);
        EXPECT_NEAR_Q(1.0 * U::s, U::s, 1.0, 0.0);
        EXPECT_NEAR_Q(1.0 * U::kg, U::kg, 1.0, 0.0);
        EXPECT_NEAR_Q(1.0 * U::mol, U::mol, 1.0, 0.0);
        EXPECT_NEAR_Q(1.0 * U::Pa, U::Pa, 1.0, 0.0);

        // Derived
        EXPECT_NEAR_Q(1.0 * U::m2, U::m2, 1.0, 0.0);
        EXPECT_NEAR_Q(1.0 * U::m3, U::m3, 1.0, 0.0);
        EXPECT_NEAR_Q(1.0 * U::m_per_s, U::m_per_s, 1.0, 0.0);
        EXPECT_NEAR_Q(1.0 * U::m_per_s2, U::m_per_s2, 1.0, 0.0);
        EXPECT_NEAR_Q(1.0 * U::N, U::N, 1.0, 0.0);
        EXPECT_NEAR_Q(1.0 * U::J, U::J, 1.0, 0.0);
        EXPECT_NEAR_Q(1.0 * U::W, U::W, 1.0, 0.0);
    }

    TEST(Units_Facade, time_convenience_minute_hour) {
        EXPECT_NEAR_Q(U::minute, U::s, 60.0, 0.0);
        EXPECT_NEAR_Q(U::hour, U::s, 3600.0, 0.0);
        EXPECT_NEAR_Q(U::hr, U::s, 3600.0, 0.0);
        EXPECT_NEAR_Q(U::min, U::s, 60.0, 0.0);
    }

    TEST(Units_Facade, length_factories) {
        EXPECT_NEAR_Q(meters(2.5), U::m, 2.5, 0.0);
        EXPECT_NEAR_Q(square_meters(3.2), U::m2, 3.2, 0.0);
        EXPECT_NEAR_Q(cubic_meters(0.75), U::m3, 0.75, 0.0);

        EXPECT_NEAR_Q(length::kilometers(1.2), U::m, 1200.0, 0.0);
        EXPECT_NEAR_Q(1.2 * length::km, U::m, 1200.0, 0.0);
    }

    TEST(Units_Facade, time_factories) {
        EXPECT_NEAR_Q(seconds(90.0), U::s, 90.0, 0.0);
        EXPECT_NEAR_Q(minutes(2.0), U::s, 120.0, 0.0);
        EXPECT_NEAR_Q(hours(1.5), U::s, 5400.0, 0.0);
    }

    TEST(Units_Facade, mass_factories) {
        EXPECT_NEAR_Q(kilograms(5.0), U::kg, 5.0, 0.0);
        EXPECT_NEAR_Q(grams(500.0), U::kg, 0.5, 1e-12);
        EXPECT_NEAR_Q(tonnes(2.0), U::kg, 2000.0, 0.0);
    }

    TEST(Units_Facade, mech_factories) {
        EXPECT_NEAR_Q(meters_per_second(12.0), U::m_per_s, 12.0, 0.0);
        EXPECT_NEAR_Q(meters_per_second2(3.5), U::m_per_s2, 3.5, 0.0);
        EXPECT_NEAR_Q(newtons(42.0), U::N, 42.0, 0.0);
        EXPECT_NEAR_Q(joules(100.0), U::J, 100.0, 0.0);
        EXPECT_NEAR_Q(watts(250.0), U::W, 250.0, 0.0);
        EXPECT_NEAR_Q(pascals(101325.0), U::Pa, 101325.0, 0.0);
    }

    TEST(Units_Facade, thermo_factories_and_utils_delta) {
        // delta-K path
        const auto dT = K_delta(10.0);
        EXPECT_NEAR_Q(dT, K_delta(1.0), 10.0, 0.0);

        // as_deltaK on a delta-K should be identity
        const auto dT2 = thermo::as_deltaK(dT);
        EXPECT_NEAR_Q(dT2, K_delta(1.0), 10.0, 0.0);
    }

    TEST(Units_Facade, thermo_absolute_and_as_deltaK) {
        // Absolute K -> delta-K
        const auto T_abs = K_absolute(300.0);
        EXPECT_NEAR_Q(T_abs - K_absolute(0.0), K_delta(1.0), 300.0, 0.0);

        const auto dT_from_abs = thermo::as_deltaK(T_abs);
        EXPECT_NEAR_Q(dT_from_abs, K_delta(1.0), 300.0, 0.0);

        // Absolute differences
        const auto T_abs2 = K_absolute(280.0);
        const auto dDiff = T_abs - T_abs2; // quantity (delta-K)
        EXPECT_NEAR_Q(dDiff, K_delta(1.0), 20.0, 0.0);
    }

    TEST(Units_Facade, literals_basic) {
        // Length, area, volume
        EXPECT_NEAR_Q(2.0_m, U::m, 2.0, 0.0);
        EXPECT_NEAR_Q(3.0_m2, U::m2, 3.0, 0.0);
        EXPECT_NEAR_Q(4.0_m3, U::m3, 4.0, 0.0);

        // Time
        EXPECT_NEAR_Q(5.0_s, U::s, 5.0, 0.0);

        // Mass
        EXPECT_NEAR_Q(6.0_kg, U::kg, 6.0, 0.0);

        // Amount & Pressure
        EXPECT_NEAR_Q(1.0_mol, U::mol, 1.0, 0.0);
        EXPECT_NEAR_Q(101325.0_Pa, U::Pa, 101325.0, 0.0);
    }

    TEST(Units_Facade, qmath_sqrt_dimensionally_correct) {
        // sqrt(4 m^2) -> 2 m
        const auto area = 4.0 * U::m2;
        const auto len = sqrt(area);
        EXPECT_NEAR_Q(len, U::m, 2.0, 0.0);
    }

    TEST(Units_Facade, near_macro_demo) {
        const auto p = pascals(101325.0);
        EXPECT_NEAR_Q(p, U::Pa, 101325.0, 1e-9);
    }

} // namespace UnitsFacadeTests
