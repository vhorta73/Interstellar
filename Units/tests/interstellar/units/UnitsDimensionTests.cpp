//#include <gtest/gtest.h>
///**
// * @file test_dimensions.hpp
// * @brief Dimension & concept conformance tests.
// * @ingroup UnitsTests
// */
//
//#include "test_config.hpp"
//
//namespace interstellar::units {
//    using namespace literals;
//
//    TEST_SUITE("[Units][Dimensions]") {
//
//        TEST_CASE("Basic concept checks compile") {
//            // These compile-time checks enforce the intended dimension categories.
//            static_assert(isq::Length<Length>);
//            static_assert(isq::Time<Time>);
//            static_assert(isq::Mass<Mass>);
//            static_assert(isq::Speed<Speed>);
//            static_assert(isq::Acceleration<Acceleration>);
//            static_assert(isq::Force<Force>);
//            static_assert(isq::Pressure<Pressure>);
//            static_assert(isq::Density<Density>);
//            static_assert(isq::Energy<Energy>);
//            static_assert(isq::Power<Power>);
//            static_assert(isq::ThermodynamicTemperature<TemperatureK>);
//            static_assert(isq::Temperature<TemperatureC>);
//            static_assert(isq::AmountOfSubstance<Amount>);
//        }
//
//        TEST_CASE("Dimensional arithmetic produces correct kinds") {
//            Length d = 1000.0_q_m;
//            Time   t = 100.0_q_s;
//
//            auto v = d / t; // Speed
//            static_assert(isq::Speed<decltype(v)>);
//
//            auto a = v / t; // Acceleration
//            static_assert(isq::Acceleration<decltype(a)>);
//
//            auto F = Mass{ 10.0 } *a; // F = m a
//            static_assert(isq::Force<decltype(F)>);
//
//            auto W = F * d; // Work/Energy
//            static_assert(isq::Energy<decltype(W)>);
//
//            (void)W;
//        }
//
//    } // TEST_SUITE
//} // namespace interstellar::units
