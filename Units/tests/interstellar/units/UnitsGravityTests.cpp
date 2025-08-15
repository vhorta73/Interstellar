//#include <gtest/gtest.h>
//
///**
// * @file test_gravity.hpp
// * @brief Gravity/kinematics sanity checks.
// * @ingroup UnitsTests
// */
//
//namespace InterstellarUnits {
//
//    TEST_SUITE("[Units][Gravity]") {
//
//        TEST_CASE("F = m a has correct magnitude and unit") {
//            Mass m = 80.0_q_kg;
//            auto F = m * g0;
//            static_assert(isq::Force<decltype(F)>);
//            CHECK(F.number() == doctest::Approx(80.0 * 9.80665));
//        }
//
//    } // TEST_SUITE
//} // namespace interstellar::units
