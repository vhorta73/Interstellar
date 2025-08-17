// Units/tests/interstellar/units/UnitsCoreTests.cpp
#include <gtest/gtest.h>
#include <type_traits>

#include "Interstellar/Units/Units.hpp"

namespace interstellar::test::units {

    // Compile-time facts: fail fast at compile time
    static_assert(std::is_same_v<Interstellar::Units::Length, decltype(1.0 * Interstellar::Units::U::m)>);
    static_assert(std::is_same_v<Interstellar::Units::Speed, decltype(1.0 * Interstellar::Units::U::m_per_s)>);
    static_assert(std::is_same_v<Interstellar::Units::Acceleration, decltype((1.0 * Interstellar::Units::U::m_per_s) / (1.0 * Interstellar::Units::U::s))>);


    TEST(UnitsCore, Speed) {
        using namespace Interstellar::Units;

        constexpr Speed v = 300.0 * U::m_per_s;

        // Scalar vs scalar: exact double equality is fine here
        EXPECT_DOUBLE_EQ(in(v, U::m_per_s), 300.0);

        // The float overload should both (a) return float and (b) carry the right value
        EXPECT_TRUE((std::is_same_v<float, decltype(in<float>(v, U::m_per_s))>));
        EXPECT_FLOAT_EQ(in<float>(v, U::m_per_s), 300.0f);
    }
}