// Units/tests/interstellar/units/UnitsConversionsTests.cpp
#include <gtest/gtest.h>

#include "Interstellar/Units/Units.hpp"
#include "helpers/UnitsTestHelpers.hpp" // EXPECT_NEAR_Q

using namespace Interstellar::Units;
using namespace Interstellar::Units::length;
using namespace Interstellar::Units::time;
using namespace Interstellar::Units::mech;
using namespace Interstellar::Units::thermo;

namespace UnitsConversionsTests {

    // ---------- Length: km <-> m ----------
    TEST(UnitsConversionsTests, length_km_to_m_and_back) {
        // 1.234 km == 1234 m
        const auto L_km = length::kilometers(1.234);

        // Compare as a quantity in metres (no numeric extraction)
        EXPECT_NEAR_Q(L_km, U::m, 1234.0, 1e-12);

        // Also verify the km scale helper constant
        EXPECT_NEAR_Q(1.234 * length::km, U::m, 1234.0, 1e-12);

        // Back "to km" via another quantity comparison: 1.234 * km
        EXPECT_NEAR_Q(L_km, U::m, 1.234 * 1000.0, 1e-12);
    }

    // ---------- Speed: km/h -> m/s ----------
    TEST(UnitsConversionsTests, speed_km_per_hour_to_m_per_s) {
        // 100 km/h = 27.777... m/s
        const auto distance = length::kilometers(100.0);
        const auto v = distance / U::hour; // Length / Time -> Speed

        EXPECT_NEAR_Q(v, U::m_per_s, 27.7777777778, 1e-9);

        // Sanity: 36 km/h = 10 m/s
        const auto v2 = length::kilometers(36.0) / U::hour;
        EXPECT_NEAR_Q(v2, U::m_per_s, 10.0, 1e-12);
    }

    // ---------- Pressure: Pa <-> bar (1 bar = 1e5 Pa) ----------
    TEST(UnitsConversionsTests, pressure_pa_bar_roundtrip) {
        constexpr double bar_to_Pa = 100000.0;

        const auto p = mech::pascals(101325.0);
        // Compare against 1.01325 bar expressed as Pa
        EXPECT_NEAR_Q(p, U::Pa, 1.01325 * bar_to_Pa, 1e-9);

        // "Roundtrip": compute a quantity from bar value * 1e5 Pa and compare as quantity
        const double bar_value = 1.01325;
        const auto p2 = mech::pascals(bar_value * bar_to_Pa);
        EXPECT_NEAR_Q(p2, U::Pa, 101325.0, 1e-9);
    }

    // ---------- Temperature bridging using facade (no Celsius type) ----------
    TEST(UnitsConversionsTests, temperature_absolute_and_deltas) {
        // 25 C = 298.15 K absolute -> compare by turning absolute into a delta
        const auto T_abs = K_absolute(273.15 + 25.0);
        EXPECT_NEAR_Q(T_abs - K_absolute(0.0), K_delta(1.0), 298.15, 1e-12);

        // delta from absolute
        const auto dT_from_abs = thermo::as_deltaK(T_abs);
        EXPECT_NEAR_Q(dT_from_abs, K_delta(1.0), 298.15, 1e-12);

        // delta between 298.15 K and 273.15 K should be 25 K
        const auto T0C_abs = K_absolute(273.15);
        const auto dTc = T_abs - T0C_abs; // quantity (delta-K)
        EXPECT_NEAR_Q(dTc, K_delta(1.0), 25.0, 1e-12);

        // Construct delta-K directly: 25 K
        const auto dT_25 = K_delta(25.0);
        EXPECT_NEAR_Q(dT_25, K_delta(1.0), 25.0, 1e-12);
    }

    // ---------- Utility-like : compare quantities directly ----------
    TEST(UnitsConversionsTests, quantity_direct_comparisons) {
        const auto L = meters(12.3);
        EXPECT_NEAR_Q(L, U::m, 12.3, 0.0);

        const auto t = minutes(2.0);
        EXPECT_NEAR_Q(t, U::s, 120.0, 0.0);

        const auto p = pascals(2500.0);
        EXPECT_NEAR_Q(p, U::Pa, 2500.0, 0.0);
    }

} // namespace UnitsConversionsTests
