//#include <gtest/gtest.h>
//
///**
// * @file test_conversions.hpp
// * @brief Unit conversion tests and quantity_cast coverage.
// * @ingroup UnitsTests
// */
//
//namespace InterstellarUnits {
//
//    TEST_SUITE("[Units][Conversions]") {
//
//        TEST_CASE("Length conversions: km <-> m") {
//            auto km = 1.234_q_km;
//            auto m = quantity_cast<si::metre>(km);
//            CHECK(m.number() == doctest::Approx(1234.0));
//
//            auto back_to_km = quantity_cast<si::kilometre>(m);
//            CHECK(back_to_km.number() == doctest::Approx(1.234));
//        }
//
//        TEST_CASE("Speed conversions: km/h -> m/s") {
//            auto v_kmh = 100.0_q_km_per_h;
//            auto v_ms = quantity_cast<si::metre_per_second>(v_kmh);
//            CHECK(v_ms.number() == doctest::Approx(27.7777777778));
//        }
//
//        TEST_CASE("Pressure conversions: Pa <-> bar") {
//            auto p = 101325.0_q_Pa;
//            auto bar = quantity_cast<si::bar>(p);
//            CHECK(bar.number() == doctest::Approx(1.01325));
//
//            auto p2 = quantity_cast<si::pascal>(bar);
//            CHECK(p2.number() == doctest::Approx(101325.0));
//        }
//
//        TEST_CASE("Temperature bridging: C <-> K") {
//            TemperatureC Tc = 25.0_q_degC;
//            auto Tk = quantity_cast<si::kelvin>(Tc);
//            CHECK(Tk.number() == doctest::Approx(298.15));
//
//            auto Tc_back = quantity_cast<si::celsius>(Tk);
//            CHECK(Tc_back.number() == doctest::Approx(25.0));
//        }
//
//    } // TEST_SUITE
//} // namespace interstellar::units
