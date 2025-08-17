// Units/tests/interstellar/units/UnitsAstronomyTests.cpp
#include <gtest/gtest.h>

#include "Interstellar/Units/Units.hpp"
#include "Interstellar/Units/Constants.hpp"
#include "Interstellar/Units/Math.hpp"
#include "helpers/UnitsTestHelpers.hpp"

/**
 * @file UnitsAstronomyTests.cpp
 * @brief Basic orbital/planetary physics checks relevant to gameplay.
 * @ingroup UnitsTests
 */

namespace interstellar::test::units {

    using namespace Interstellar::Units;

    // --- Compile-time dimensional checks (avoid exact-type equality) ---
    static_assert(std::same_as<
        decltype(kStandardGravity / (1.0 * U::m_per_s2)),
        decltype(1.0 * ::mp_units::one)>);

    static_assert(std::same_as<
        decltype(kEarthMu / (1.0 * (U::m3 / U::s2))),
        decltype(1.0 * ::mp_units::one)>);

    static_assert(std::same_as<
        decltype(kEarthRadius / (1.0 * U::m)),
        decltype(1.0 * ::mp_units::one)>);

    TEST(UnitsAstronomy, g0_greater_than_one_mps2) {
        // Compare using quantities to keep dimensional safety.
        const auto one_m_per_s2 = mech::meters_per_second2(1.0);
        EXPECT_GT(kStandardGravity, one_m_per_s2);
    }

    TEST(UnitsAstronomy, earth_escape_velocity_near_surface_approx_11186_m_s) {
        const auto ve = escape_velocity(kEarthMu, kEarthRadius);   // quantity (m/s)

        // Optional: print computed value on failure
        SCOPED_TRACE(::testing::Message() << "ve = " << ve);

        // Relative check: 11,186 m/s, tolerance = +/-2%
        EXPECT_NEAR_Q_REL(ve, U::m_per_s, 11186.0, 0.02);
    }

} // namespace interstellar::test::units
