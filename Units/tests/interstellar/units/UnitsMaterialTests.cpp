// Units/tests/interstellar/units/UnitsMaterialsTests.cpp
#include <gtest/gtest.h>

#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Math.hpp"
#include "Interstellar/Units/Constants.hpp"
#include "helpers/UnitsTestHelpers.hpp" // EXPECT_NEAR_Q_ABS / EXPECT_NEAR_Q_REL

namespace interstellar::test::units {

    using namespace Interstellar::Units;
    using namespace Interstellar::Units::mass;
    using namespace Interstellar::Units::length;
    using namespace Interstellar::Units::mech;
    using namespace Interstellar::Units::thermo;

    TEST(UnitsMaterials, coefficients_are_unit_free_scalars) {
        // Pure scalars (dimensionless)
        const double mu_static = 0.60; // coefficient of friction
        const double mu_dynamic = 0.50;
        const double albedo = 0.12; // reflectivity
        const double Cd = 1.00; // drag coefficient (blunt-ish body)

        // Basic ranges
        EXPECT_GE(mu_static, 0.0);
        EXPECT_GE(mu_dynamic, 0.0);

        EXPECT_GE(albedo, 0.0);
        EXPECT_LE(albedo, 1.0);

        EXPECT_GT(Cd, 0.0); // typical Cd > 0
    }

    TEST(UnitsMaterials, friction_force_has_newton_units_and_correct_magnitude) {
        // Normal force N = m * g0  (quantity)
        const auto m = kilograms(80.0);
        const auto N = m * kStandardGravity; // newtons

        // Friction force F_f = mu * N, mu is scalar
        const double mu_dynamic = 0.50;
        const auto   F_f = mu_dynamic * N;

        SCOPED_TRACE(::testing::Message() << "F_f = " << F_f);

        // Expected magnitude: 0.5 * 80 * 9.80665 = 392.266 N
        EXPECT_NEAR_Q_ABS(F_f, U::N, 0.5 * 80.0 * 9.80665, 1e-9);
    }

    TEST(UnitsMaterials, drag_force_example_is_in_newtons) {
        // F_d = 0.5 * rho * v^2 * Cd * A
        const auto rho = 1.225 * (U::kg / U::m3); // air density at sea level
        const auto v = meters(10.0) / U::s;     // 10 m/s
        const auto A = square_meters(0.10);     // 0.10 m^2 reference area
        const double Cd = 1.0;                    // scalar coefficient

        const auto Fd = 0.5 * rho * v * v * Cd * A; // quantity -> newtons

        SCOPED_TRACE(::testing::Message() << "Fd = " << Fd);

        // Numeric check: 0.5 * 1.225 * 100 * 1 * 0.1 = 6.125 N
        EXPECT_NEAR_Q_ABS(Fd, U::N, 6.125, 1e-12);
    }

    TEST(UnitsMaterials, albedo_affects_equilibrium_temperature_monotonically) {
        // Use W/m^2 (irradiance), not W (power)
        const auto S_flux = kSolarConstant;  // irradiance in W*m^-2 (quantity)

        // A = 0 -> 278.3 K
        const auto T_A0 = equilibrium_temp(S_flux, 0.0);
        SCOPED_TRACE(::testing::Message() << "T_A0  = " << T_A0);

        EXPECT_NEAR_Q_ABS(T_A0, K_delta(1.0), 278.3, 0.6);   // +/-0.6 K

        // Earth-like A 0.3 -> 254 K (no greenhouse)
        const auto T_A03 = equilibrium_temp(S_flux, 0.3);
        SCOPED_TRACE(::testing::Message() << "T_A03 = " << T_A03);

        EXPECT_NEAR_Q_ABS(T_A03, K_delta(1.0), 254.0, 1.0);  // +/-1.0 K

        // Brighter A = 0.6 -> cooler (monotonicity)
        const auto T_A06 = equilibrium_temp(S_flux, 0.6);
        SCOPED_TRACE(::testing::Message() << "T_A06 = " << T_A06);

        EXPECT_LT(T_A06, T_A03);
        EXPECT_LT(T_A03, T_A0);
    }

}
