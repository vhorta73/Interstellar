#include <gtest/gtest.h>
#include <type_traits>

#include "Interstellar/Units/Units.hpp"

namespace UnitsCoreTests {

    TEST(UnitsCoreTests, Speed) {
        EXPECT_TRUE(true);
        //EXPECT_EQ(std::is_same_v<Length, decltype(1.0 * U::m)>);
        //EXPECT_EQ(std::is_same_v<Speed, decltype(1.0 * U::m_per_s)>);

        //constexpr Speed v = 300.0 * U::m_per_s;
        //EXPECT_EQ(in(v, U::m_per_s) == 300.0);
        //EXPECT_EQ(std::is_same_v<float, decltype(in<float>(v, U::m_per_s))>);
    }
}