// Units/tests/UnitsDimensionTests.cpp
#include <gtest/gtest.h>
#include "Interstellar/Units/Units.hpp"
#include "helpers/UnitsTestHelpers.hpp" // EXPECT_NEAR_Q_ABS / EXPECT_NEAR_Q_REL / EXPECT_NEAR_Q_REL_FLOOR

namespace interstellar::test::units {

    // Short aliases to avoid broad file-scope using
    namespace UQ = Interstellar::Units;     // quantities/types/factories
    namespace U = Interstellar::Units::U;  // unit symbols (m, s, Pa, ...)

    // bring in the small domain factories for readability (scoped to this TU/namespace)
    using UQ::length::meters;        // meters(), square_meters(), cubic_meters(), km
    using UQ::length::square_meters;
    using UQ::length::cubic_meters;
    using UQ::length::km;
    using UQ::length::kilometers;
    using UQ::time::seconds;         // seconds(), minutes(), hours()
    using UQ::time::minutes;
    using UQ::time::hours;
    using UQ::mass::kilograms;       // kilograms()
    using UQ::mech::pascals;         // pascals(), meters_per_second(), ...
    using UQ::mech::meters_per_second;
    using UQ::mech::meters_per_second2;
    using UQ::mech::newtons;
    using UQ::mech::joules;
    using UQ::mech::watts;
    using UQ::thermo::K_delta;       // K_delta(), K_absolute(), as_deltaK()
    using UQ::thermo::K_absolute;
    using UQ::thermo::as_deltaK;

    // ==============================
    // Compile-time semantic checks
    // ==============================

    // Base units: ensure facade aliases are sane
    static_assert(std::is_same_v<UQ::Length, decltype(1.0 * U::m)>);
    static_assert(std::is_same_v<UQ::Time, decltype(1.0 * U::s)>);
    static_assert(std::is_same_v<UQ::Mass, decltype(1.0 * U::kg)>);
    static_assert(std::is_same_v<UQ::Amount, decltype(1.0 * U::mol)>);

    // Common derived aliases
    static_assert(std::is_same_v<UQ::Area, decltype(1.0 * U::m2)>);
    static_assert(std::is_same_v<UQ::Volume, decltype(1.0 * U::m3)>);
    static_assert(std::is_same_v<UQ::Pressure, decltype(1.0 * U::Pa)>);

    // Dimensional arithmetic shapes: v = L/T, a = v/T, rho = M/V
    using V = decltype(std::declval<UQ::Length>() / std::declval<UQ::Time>());
    using A = decltype(std::declval<V>() / std::declval<UQ::Time>());
    using Rho = decltype(std::declval<UQ::Mass>() / std::declval<UQ::Volume>());

    static_assert(std::is_same_v<V, UQ::Speed>);
    static_assert(std::is_same_v<A, UQ::Acceleration>);
    static_assert(std::is_same_v<Rho, UQ::Density>);

    // Temperature normalization yields delta-K
    static_assert(std::is_same_v<decltype(as_deltaK(std::declval<UQ::TemperaturePointK>())), UQ::TemperatureDeltaK>);
    static_assert(std::is_same_v<decltype(as_deltaK(std::declval<UQ::TemperatureDeltaK>())), UQ::TemperatureDeltaK>);

    // ==============================
    // Runtime sanity checks
    // ==============================

    TEST(UnitsDimensions, time_convenience_values) {
        // Exact identities via absolute-tolerance compare (unit-safe)
        EXPECT_NEAR_Q_ABS(U::minute, U::s, 60.0, 0.0);
        EXPECT_NEAR_Q_ABS(U::hour, U::s, 3600.0, 0.0);

        // Factory helpers
        EXPECT_NEAR_Q_ABS(minutes(2.0), U::s, 120.0, 0.0);
        EXPECT_NEAR_Q_ABS(hours(0.5), U::s, 1800.0, 0.0);
    }

    TEST(UnitsDimensions, length_area_volume_factories) {
        const auto L = meters(3.0);
        const auto A = square_meters(4.0);
        const auto Vv = cubic_meters(5.0);

        EXPECT_NEAR_Q_ABS(L, U::m, 3.0, 0.0);
        EXPECT_NEAR_Q_ABS(A, U::m2, 4.0, 0.0);
        EXPECT_NEAR_Q_ABS(Vv, U::m3, 5.0, 0.0);

        // kilometer convenience constant
        EXPECT_NEAR_Q_ABS(2.5 * km, U::m, 2500.0, 0.0);
        EXPECT_NEAR_Q_ABS(kilometers(2.5), U::m, 2500.0, 0.0);
    }

    TEST(UnitsDimensions, mechanics_dimensions) {
        const auto d = meters(100.0);
        const auto t = seconds(10.0);

        const auto v = d / t;   // Speed
        EXPECT_NEAR_Q_ABS(v, U::m_per_s, 10.0, 1e-12);

        const auto a = v / t;   // Acceleration
        EXPECT_NEAR_Q_ABS(a, U::m_per_s2, 1.0, 1e-12);

        const auto m = kilograms(2.0);
        const auto Fq = m * a;  // Force
        EXPECT_NEAR_Q_ABS(Fq, U::N, 2.0, 1e-12);

        const auto Wq = Fq * d; // Energy (work)
        EXPECT_NEAR_Q_ABS(Wq, U::J, 200.0, 1e-12);
    }

    TEST(UnitsDimensions, density_dimension) {
        const auto m = kilograms(1000.0);
        const auto Vv = cubic_meters(1.25);
        const auto rho = m / Vv;               // kg/m^3

        // Use the composite unit expression (kg/m^3) explicitly
        EXPECT_NEAR_Q_ABS(rho, U::kg / U::m3, 800.0, 1e-12);
    }

    TEST(UnitsDimensions, thermo_normalization_and_differences) {
        const auto T_abs = K_absolute(300.0);
        const auto T0 = K_absolute(273.15);

        // as_deltaK on absolute returns delta-K of same magnitude
        const auto dT_abs = as_deltaK(T_abs);
        EXPECT_NEAR_Q_ABS(dT_abs, K_delta(1.0), 300.0, 0.0);

        // absolute difference between two absolute temperatures is a delta-K
        const auto diff = T_abs - T0;
        EXPECT_NEAR_Q_ABS(diff, K_delta(1.0), 26.85, 1e-12);

        const auto d25 = K_delta(25.0);
        EXPECT_NEAR_Q_ABS(d25, K_delta(1.0), 25.0, 0.0);
    }

    TEST(UnitsDimensions, power_dimension) {
        const auto energy = joules(120.0);
        const auto duration = seconds(60.0);
        const auto P = energy / duration;      // Power

        EXPECT_NEAR_Q_ABS(P, U::W, 2.0, 1e-12);
    }

}