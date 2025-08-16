// Units/tests/UnitsDimensionTests.cpp
#include <gtest/gtest.h>
#include "Interstellar/Units/Units.hpp"

using namespace Interstellar::Units;

// bring in the small domain factories for readability
using namespace Interstellar::Units::length;  // meters(), square_meters(), cubic_meters(), km
using namespace Interstellar::Units::time;    // seconds(), minutes(), hours()
using namespace Interstellar::Units::mass;    // kilograms()
using namespace Interstellar::Units::mech;    // pascals(), meters_per_second(), ...
using namespace Interstellar::Units::thermo;  // K_delta(), K_absolute(), as_deltaK()

namespace UnitsDimensionsTests {

    // Tiny relative tolerance helper (quantity-vs-quantity, no numeric extraction)
    template<class Q1, class Q2>
    void expect_near_q(const Q1& actual, const Q2& expected, double rel_tol) {
        // Both sides must be comparable (same dimension); if not, it fails to compile.
        EXPECT_TRUE(detail::mu::abs(actual - expected) < (rel_tol * expected));
    }

    // ==============================
    // Compile-time semantic checks
    // ==============================

    // Base units: just ensure the types compile as quantities in our facade
    static_assert(std::is_same_v<Length, decltype(1.0 * U::m)>);
    static_assert(std::is_same_v<Time, decltype(1.0 * U::s)>);
    static_assert(std::is_same_v<Mass, decltype(1.0 * U::kg)>);
    static_assert(std::is_same_v<Amount, decltype(1.0 * U::mol)>);

    // Common derived aliases
    static_assert(std::is_same_v<Area, decltype(1.0 * U::m2)>);
    static_assert(std::is_same_v<Volume, decltype(1.0 * U::m3)>);
    static_assert(std::is_same_v<Pressure, decltype(1.0 * U::Pa)>);

    // Dimensional arithmetic shapes: v = L/T, a = v/T, rho = M/V
    using V = decltype(std::declval<Length>() / std::declval<Time>());
    using A = decltype(std::declval<V>() / std::declval<Time>());
    using Rho = decltype(std::declval<Mass>() / std::declval<Volume>());

    static_assert(std::is_same_v<V, Speed>);
    static_assert(std::is_same_v<A, Acceleration>);
    static_assert(std::is_same_v<Rho, Density>);

    // Temperature normalization yields delta-K
    static_assert(std::is_same_v<decltype(as_deltaK(std::declval<TemperaturePointK>())), TemperatureDeltaK>);
    static_assert(std::is_same_v<decltype(as_deltaK(std::declval<TemperatureDeltaK>())), TemperatureDeltaK>);

    // ==============================
    // Runtime sanity checks
    // ==============================

    TEST(UnitsDimensionsTests, time_convenience_values) {
        // Exact identities
        EXPECT_TRUE(U::minute == 60.0 * U::s);
        EXPECT_TRUE(U::hour == 60.0 * U::minute);

        // Factory helpers
        EXPECT_TRUE(minutes(2.0) == 120.0 * U::s);
        EXPECT_TRUE(hours(0.5) == 1800.0 * U::s);
    }

    TEST(UnitsDimensionsTests, length_area_volume_factories) {
        const auto L = meters(3.0);
        const auto A = square_meters(4.0);
        const auto Vv = cubic_meters(5.0);

        EXPECT_TRUE(L == 3.0 * U::m);
        EXPECT_TRUE(A == 4.0 * U::m2);
        EXPECT_TRUE(Vv == 5.0 * U::m3);

        // kilometer convenience constant
        EXPECT_TRUE(2.5 * length::km == 2500.0 * U::m);
        EXPECT_TRUE(kilometers(2.5) == 2500.0 * U::m);
    }

    TEST(UnitsDimensionsTests, mechanics_dimensions) {
        const auto d = meters(100.0);
        const auto t = seconds(10.0);

        const auto v = d / t;   // Speed
        expect_near_q(v, 10.0 * U::m_per_s, 1e-12);

        const auto a = v / t;   // Acceleration
        expect_near_q(a, 1.0 * U::m_per_s2, 1e-12);

        const auto m = kilograms(2.0);
        const auto Fq = m * a;  // Force
        expect_near_q(Fq, 2.0 * U::N, 1e-12);

        const auto Wq = Fq * d; // Energy (work)
        expect_near_q(Wq, 200.0 * U::J, 1e-12);
    }

    TEST(UnitsDimensionsTests, density_dimension) {
        const auto m = kilograms(1000.0);
        const auto Vv = cubic_meters(1.25);
        const auto rho = m / Vv;               // kg/m^3

        expect_near_q(rho, 800.0 * (U::kg / U::m3), 1e-12);
    }

    TEST(UnitsDimensionsTests, thermo_normalization_and_differences) {
        const auto T_abs = K_absolute(300.0);
        const auto T0 = K_absolute(273.15);

        // as_deltaK on absolute returns delta-K of the same magnitude (from absolute zero)
        const auto dT_abs = as_deltaK(T_abs);
        EXPECT_TRUE(dT_abs == K_delta(300.0));

        // absolute difference between two absolute temperatures is a delta-K
        const auto diff = T_abs - T0;
        expect_near_q(diff, K_delta(26.85), 1e-12);

        const auto d25 = K_delta(25.0);
        EXPECT_TRUE(d25 == K_delta(25.0));
    }

    TEST(UnitsDimensionsTests, power_dimension) {
        const auto energy = joules(120.0);
        const auto duration = seconds(60.0);
        const auto P = energy / duration;      // Power

        expect_near_q(P, 2.0 * U::W, 1e-12);
    }

} // namespace UnitsDimensionsTests
