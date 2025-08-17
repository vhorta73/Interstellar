// Units/tests/interstellar/units/UnitsFacadeTests.cpp
#include <gtest/gtest.h>

#include "Interstellar/Units/Units.hpp"
#include "helpers/UnitsTestHelpers.hpp"  // EXPECT_NEAR_Q_ABS / EXPECT_NEAR_Q_REL / EXPECT_NEAR_Q_REL_FLOOR

namespace interstellar::test::units {

    // Short, explicit aliases to avoid broad 'using namespace'
    namespace UQ = Interstellar::Units;          // quantities/types/factories
    namespace U = Interstellar::Units::U;      // unit symbols (m, s, Pa, ...)
    using UQ::length::meters;
    using UQ::length::square_meters;
    using UQ::length::cubic_meters;
    using UQ::length::km;
    using UQ::length::kilometers;
    using UQ::time::seconds;
    using UQ::time::minutes;
    using UQ::time::hours;
    using UQ::mass::kilograms;
    using UQ::mech::meters_per_second;
    using UQ::mech::meters_per_second2;
    using UQ::mech::newtons;
    using UQ::mech::joules;
    using UQ::mech::watts;
    using UQ::mech::pascals;
    using UQ::thermo::K_delta;
    using UQ::thermo::K_absolute;
    using UQ::thermo::as_deltaK;
    using UQ::qmath::sqrt;
    using UQ::literals::operator"" _mol;
    using UQ::literals::operator"" _m;
    using UQ::literals::operator"" _m2;
    using UQ::literals::operator"" _m3;
    using UQ::literals::operator"" _s;
    using UQ::literals::operator"" _kg;
    using UQ::literals::operator"" _Pa;

    TEST(UnitsFacade, types_exist_and_basic_construction) {
        const UQ::Length  L = meters(2.0);
        const UQ::Area    A = square_meters(3.0);
        const UQ::Volume  V = cubic_meters(4.0);
        const UQ::Time    T = seconds(5.0);
        const UQ::Mass    M = kilograms(6.0);
        const UQ::Amount  N = 1.0_mol;

        (void)L; (void)A; (void)V; (void)T; (void)M; (void)N;
        SUCCEED();
    }

    TEST(UnitsFacade, U_catalog_base_and_derived) {
        // Base
        EXPECT_NEAR_Q_ABS(1.0 * U::m, U::m, 1.0, 0.0);
        EXPECT_NEAR_Q_ABS(1.0 * U::s, U::s, 1.0, 0.0);
        EXPECT_NEAR_Q_ABS(1.0 * U::kg, U::kg, 1.0, 0.0);
        EXPECT_NEAR_Q_ABS(1.0 * U::mol, U::mol, 1.0, 0.0);
        EXPECT_NEAR_Q_ABS(1.0 * U::Pa, U::Pa, 1.0, 0.0);

        // Derived
        EXPECT_NEAR_Q_ABS(1.0 * U::m2, U::m2, 1.0, 0.0);
        EXPECT_NEAR_Q_ABS(1.0 * U::m3, U::m3, 1.0, 0.0);
        EXPECT_NEAR_Q_ABS(1.0 * U::m_per_s, U::m_per_s, 1.0, 0.0);
        EXPECT_NEAR_Q_ABS(1.0 * U::m_per_s2, U::m_per_s2, 1.0, 0.0);
        EXPECT_NEAR_Q_ABS(1.0 * U::N, U::N, 1.0, 0.0);
        EXPECT_NEAR_Q_ABS(1.0 * U::J, U::J, 1.0, 0.0);
        EXPECT_NEAR_Q_ABS(1.0 * U::W, U::W, 1.0, 0.0);
    }

    TEST(UnitsFacade, time_convenience_minute_hour) {
        EXPECT_NEAR_Q_ABS(U::minute, U::s, 60.0, 0.0);
        EXPECT_NEAR_Q_ABS(U::hour, U::s, 3600.0, 0.0);
        EXPECT_NEAR_Q_ABS(U::hr, U::s, 3600.0, 0.0);
        EXPECT_NEAR_Q_ABS(U::min, U::s, 60.0, 0.0);
    }

    TEST(UnitsFacade, length_factories) {
        EXPECT_NEAR_Q_ABS(meters(2.5), U::m, 2.5, 0.0);
        EXPECT_NEAR_Q_ABS(square_meters(3.2), U::m2, 3.2, 0.0);
        EXPECT_NEAR_Q_ABS(cubic_meters(0.75), U::m3, 0.75, 0.0);

        EXPECT_NEAR_Q_ABS(kilometers(1.2), U::m, 1200.0, 0.0);
        EXPECT_NEAR_Q_ABS(1.2 * km, U::m, 1200.0, 0.0);
    }

    TEST(UnitsFacade, time_factories) {
        EXPECT_NEAR_Q_ABS(seconds(90.0), U::s, 90.0, 0.0);
        EXPECT_NEAR_Q_ABS(minutes(2.0), U::s, 120.0, 0.0);
        EXPECT_NEAR_Q_ABS(hours(1.5), U::s, 5400.0, 0.0);
    }

    TEST(UnitsFacade, mass_factories) {
        EXPECT_NEAR_Q_ABS(kilograms(5.0), U::kg, 5.0, 0.0);
        EXPECT_NEAR_Q_ABS(UQ::mass::grams(500.0), U::kg, 0.5, 1e-12);
        EXPECT_NEAR_Q_ABS(UQ::mass::tonnes(2.0), U::kg, 2000.0, 0.0);
    }

    TEST(UnitsFacade, mech_factories) {
        EXPECT_NEAR_Q_ABS(meters_per_second(12.0), U::m_per_s, 12.0, 0.0);
        EXPECT_NEAR_Q_ABS(meters_per_second2(3.5), U::m_per_s2, 3.5, 0.0);
        EXPECT_NEAR_Q_ABS(newtons(42.0), U::N, 42.0, 0.0);
        EXPECT_NEAR_Q_ABS(joules(100.0), U::J, 100.0, 0.0);
        EXPECT_NEAR_Q_ABS(watts(250.0), U::W, 250.0, 0.0);
        EXPECT_NEAR_Q_ABS(pascals(101325.0), U::Pa, 101325.0, 0.0);
    }

    TEST(UnitsFacade, thermo_factories_and_utils_delta) {
        const auto dT = K_delta(10.0);
        EXPECT_NEAR_Q_ABS(dT, K_delta(1.0), 10.0, 0.0);

        const auto dT2 = as_deltaK(dT);
        EXPECT_NEAR_Q_ABS(dT2, K_delta(1.0), 10.0, 0.0);
    }

    TEST(UnitsFacade, thermo_absolute_and_as_deltaK) {
        const auto T_abs = K_absolute(300.0);
        EXPECT_NEAR_Q_ABS(T_abs - K_absolute(0.0), K_delta(1.0), 300.0, 0.0);

        const auto dT_from_abs = as_deltaK(T_abs);
        EXPECT_NEAR_Q_ABS(dT_from_abs, K_delta(1.0), 300.0, 0.0);

        const auto T_abs2 = K_absolute(280.0);
        const auto dDiff = T_abs - T_abs2; // quantity (delta-K)
        EXPECT_NEAR_Q_ABS(dDiff, K_delta(1.0), 20.0, 0.0);
    }

    TEST(UnitsFacade, literals_basic) {
        EXPECT_NEAR_Q_ABS(2.0_m, U::m, 2.0, 0.0);
        EXPECT_NEAR_Q_ABS(3.0_m2, U::m2, 3.0, 0.0);
        EXPECT_NEAR_Q_ABS(4.0_m3, U::m3, 4.0, 0.0);
        EXPECT_NEAR_Q_ABS(5.0_s, U::s, 5.0, 0.0);
        EXPECT_NEAR_Q_ABS(6.0_kg, U::kg, 6.0, 0.0);
        EXPECT_NEAR_Q_ABS(1.0_mol, U::mol, 1.0, 0.0);
        EXPECT_NEAR_Q_ABS(101325.0_Pa, U::Pa, 101325.0, 0.0);
    }

    TEST(UnitsFacade, qmath_sqrt_dimensionally_correct) {
        const auto area = 4.0 * U::m2;
        const auto len = sqrt(area);
        EXPECT_NEAR_Q_ABS(len, U::m, 2.0, 0.0);
    }

    TEST(UnitsFacade, near_macro_demo) {
        const auto p = pascals(101325.0);
        EXPECT_NEAR_Q_ABS(p, U::Pa, 101325.0, 1e-9);
    }

    // Optional: unit catalog sanity
    TEST(UnitsFacade, unit_catalog_composite_aliases) {
        // 1 * (W/m^2) is the same quantity as (1 W) / (1 m^2)
        EXPECT_NEAR_Q_ABS(1.0 * U::W_per_m2, U::W / U::m2, 1.0, 0.0);
    }

    // Optional: compile-time shape check (doesn't run; fails fast if facade changes)
    static_assert(std::is_same_v<
        decltype((1.0 * U::m2) / (1.0 * U::m)),  // sqrt-dimension example target
        decltype(1.0 * U::m)
    >);

}