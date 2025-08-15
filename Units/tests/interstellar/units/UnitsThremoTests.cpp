//#include <gtest/gtest.h>
//
///**
// * @file UnitsThermoTests.hpp
// * @brief Thermodynamics: ideal gas law, temperature conversions, humidity.
// * @ingroup UnitsTests
// */
//
//namespace InterstellarUnits {
//
//    TEST("[Units][Thermo]","Ideal gas law : 1 mol at STP in 22.414e-3 m3 + / -101325 Pa") {
//        Amount n = 1.0_q_mol;
//        TemperatureK T = 273.15_q_K;
//        Volume V = 22.414e-3_q_m3;
//
//        auto P = ideal_gas_pressure(n, T, V);
//        EXPECT_TRUE(P.number() == doctest::Approx(101325.0).epsilon(0.01)); // +/-1% tol for molar volume rounding
//    }
//
//    TEST("[Units][Thermo]", "Tetens saturation vapor pressure at 20C approx. 2338 Pa") {
//        TemperatureC Tc = 20.0_q_degC;
//        auto es = saturation_vapor_pressure_tetens(Tc);
//        EXPECT_TRUE(es.number() == doctest::Approx(2338.0).epsilon(0.03)); // +/-3% error allowance
//    }
//
//    TEST("[Units][Thermo]","Relative humidity to partial pressure") {
//        TemperatureC Tc = 25.0_q_degC;
//        auto es = saturation_vapor_pressure_tetens(Tc); // Approx 3169 Pa
//        auto e = vapor_partial_pressure(0.5, es);
//        EXPECT_TRUE(e.number() == doctest::Approx(es.number() * 0.5));
//    }
//}