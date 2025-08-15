//#include <gtest/gtest.h>
//
///**
// * @file test_fluids.hpp
// * @brief Fluid statics/dynamics sanity checks.
// * @ingroup UnitsTests
// */
//
//namespace InterstellarUnits {
//
//    TEST_SUITE("[Units][Fluids]") {
//
//        TEST_CASE("Hydrostatic pressure head: rho g h") {
//            Density rho = 1000.0_q_kg_per_m3; // fresh water
//            Length  h = 10.0_q_m;
//            auto dP = hydrostatic_pressure(rho, g0, h);
//
//            // Expect ~98.0665 kPa at 10 m depth (ignoring atmosphere)
//            CHECK(dP.number() == doctest::Approx(98'066.5));
//        }
//
//        TEST_CASE("Reynolds number is dimensionless") {
//            // Re = p v L / mu. We'll only verify dimensionality by constructing a ratio
//            // that reduces to a pure scalar (no compile-time unit).
//            Density rho = 1.225_q_kg_per_m3;     // air / sea level
//            Speed   v = 10.0_q_m_per_s;
//            Length  L = 0.1_q_m;
//            auto    mu = 1.8e-5_q_Pa * 1.0_q_s; // dynamic viscosity (Pa/s-1)
//
//            auto Re = (rho * v * L) / mu;
//            static_assert(std::is_same_v<decltype(Re), double> || std::is_arithmetic_v<decltype(Re)>);
//            (void)Re;
//        }
//
//    } // TEST_SUITE
//} // namespace interstellar::units
