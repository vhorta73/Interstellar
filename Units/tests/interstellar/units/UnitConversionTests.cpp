// Units/tests/interstellar/units/UnitsConversionsTests.cpp
#include <gtest/gtest.h>

#include "Interstellar/Units/Units.hpp"
#include "helpers/UnitsTestHelpers.hpp" // EXPECT_NEAR_Q_ABS / EXPECT_NEAR_Q_REL / EXPECT_NEAR_Q_REL_FLOOR

namespace interstellar::test::units {

    // Pull in what you need INSIDE the test namespace
    using namespace Interstellar::Units;
    using namespace Interstellar::Units::length;
    using namespace Interstellar::Units::time;
    using namespace Interstellar::Units::mech;
    using namespace Interstellar::Units::thermo;

    // ---------- Length: km <-> m ----------
    TEST(UnitsConversions, length_km_to_m_and_back) {
        const auto L_km = length::kilometers(1.234);

        EXPECT_NEAR_Q_ABS(L_km, U::m, 1234.0, 1e-12);
        EXPECT_NEAR_Q_ABS(1.234 * length::km, U::m, 1234.0, 1e-12);
        EXPECT_NEAR_Q_ABS(L_km, U::m, 1.234 * 1000.0, 1e-12);
    }

    // ---------- Speed: km/h -> m/s ----------
    TEST(UnitsConversions, speed_km_per_hour_to_m_per_s) {
        const auto distance = length::kilometers(100.0);
        const auto v = distance / U::hour; // Length / Time -> Speed

        EXPECT_NEAR_Q_ABS(v, U::m_per_s, 27.7777777778, 1e-9);

        const auto v2 = length::kilometers(36.0) / U::hour;
        EXPECT_NEAR_Q_ABS(v2, U::m_per_s, 10.0, 1e-12);
    }

    // ---------- Pressure: Pa <-> bar (1 bar = 1e5 Pa) ----------
    TEST(UnitsConversions, pressure_pa_bar_roundtrip) {
        constexpr double bar_to_Pa = 100000.0;

        const auto p = mech::pascals(101325.0);
        EXPECT_NEAR_Q_ABS(p, U::Pa, 1.01325 * bar_to_Pa, 1e-9);

        const double bar_value = 1.01325;
        const auto p2 = mech::pascals(bar_value * bar_to_Pa);
        EXPECT_NEAR_Q_ABS(p2, U::Pa, 101325.0, 1e-9);
    }

    // ---------- Temperature bridging using facade (no Celsius type) ----------
    TEST(UnitsConversions, temperature_absolute_and_deltas) {
        const auto T_abs = K_absolute(273.15 + 25.0);
        EXPECT_NEAR_Q_ABS(T_abs - K_absolute(0.0), K_delta(1.0), 298.15, 1e-12);

        const auto dT_from_abs = thermo::as_deltaK(T_abs);
        EXPECT_NEAR_Q_ABS(dT_from_abs, K_delta(1.0), 298.15, 1e-12);

        const auto T0C_abs = K_absolute(273.15);
        const auto dTc = T_abs - T0C_abs;
        EXPECT_NEAR_Q_ABS(dTc, K_delta(1.0), 25.0, 1e-12);

        const auto dT_25 = K_delta(25.0);
        EXPECT_NEAR_Q_ABS(dT_25, K_delta(1.0), 25.0, 1e-12);
    }

    // ---------- Utility-like : compare quantities directly ----------
    TEST(UnitsConversions, quantity_direct_comparisons) {
        const auto L = meters(12.3);
        EXPECT_NEAR_Q_ABS(L, U::m, 12.3, 0.0);

        const auto t = minutes(2.0);
        EXPECT_NEAR_Q_ABS(t, U::s, 120.0, 0.0);

        const auto p = pascals(2500.0);
        EXPECT_NEAR_Q_ABS(p, U::Pa, 2500.0, 0.0);
    }

}