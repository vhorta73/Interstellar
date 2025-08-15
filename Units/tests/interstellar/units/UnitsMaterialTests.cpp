//#include <gtest/gtest.h>
///**
// * @file test_materials.hpp
// * @brief Dimensionless gameplay properties (e.g., friction, albedo) consistency checks.
// * @ingroup UnitsTests
// */
//
//namespace InterstellarUnits {
//
//    TEST_SUITE("[Units][Materials]") {
//
//        TEST_CASE("Coefficients are unit-free scalars") {
//            // Coefficient of friction (mu), albedo, etc. must be pure scalars in [0,1] or appropriate ranges.
//            const double mu_static = 0.6;  // e.g., rubber on concrete
//            const double mu_dynamic = 0.5;
//            const double albedo = 0.12; // basaltic rock order-of-magnitude
//
//            CHECK(mu_static >= 0.0);
//            CHECK(mu_dynamic >= 0.0);
//            CHECK(albedo >= 0.0);
//            CHECK(albedo <= 1.0);
//        }
//
//    } // TEST_SUITE
//} // namespace interstellar::units
