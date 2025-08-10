//#include <gtest/gtest.h>
//#include "nlohmann/json.hpp"
//#include "Interstellar/Config/JsonImpl/ElementConfig.hpp"
//#include "Interstellar/Data/ElementData.hpp"
//
//// NEW: shared helpers (EPS, optional-aware comparisons, file IO)
//#include "TestSupport.hpp"
//
//using nlohmann::json;
//using Interstellar::Config::JsonImpl::ElementConfig;
//using Interstellar::Data::ElementData;
//using namespace TestSupport;

//namespace Interstellar_Config_JsonImpl_ElementData_Tests {
//
//    TEST(ElementConfig_Smoke, Json_ToData_RoundTrip_Single) {
//        json input = {
//            {"Symbol", "Xe"},
//            {"Name", "Xenon"},
//            {"AtomicNumber", 54},
//            {"AtomicMass", 131.29f},
//            {"Group", 18},
//            {"Period", 5},
//            {"Block", "p"},
//            {"StandardState", "gas"},
//            {"Color", {0.8f, 0.9f, 1.0f}},
//            {"CosmicAbundance", {{"Value", 8e-6f}, {"Unit", "ppm"}}},
//            {"ElectronConfiguration", "[Kr] 4d10 5s2 5p6"},
//            {"ElectronShells", {2, 8, 18, 18, 8}},
//            {"ValenceElectrons", 8},
//            {"ChemicalProperties", {
//                {"Electronegativity", {{"Value", 2.6f}, {"Unit", "Pauling"}}},
//                {"IonizationEnergy",  {{"First", {{"Value", 12.13f}, {"Unit","eV"}}}}},
//                {"ElectronAffinity",  {{"Value", 0.0f}, {"Unit","eV"}}},
//                {"Reactivity",        {{"Value", 0.1f}, {"Scale","0-1"}}}
//            }},
//            {"PhysicalProperties", {
//                {"Density", {{"Value", 0.0059f}, {"Unit","g/cm^3"}}}
//            }}
//        };
//
//        ElementConfig cfg;
//        ASSERT_NO_THROW(cfg.fromJson(input));
//        ElementData d = cfg.toData();
//
//        EXPECT_EQ("Xe", d.symbol);
//        EXPECT_EQ("Xenon", d.name);
//        EXPECT_EQ(54, d.atomicNumber);
//        EXPECT_NEAR(131.29f, d.atomicMass, kEPS);
//
//        // optional-aware compare
//        expect_opt_float_near_optopt(d.cosmicAbundance.value,
//            std::optional<float>(8e-6f),
//            kEPS, "CosmicAbundance.Value");
//        EXPECT_EQ("ppm", d.cosmicAbundance.unit);
//
//        EXPECT_NEAR(0.8f, d.colour.x, kEPS);
//        EXPECT_NEAR(0.9f, d.colour.y, kEPS);
//        EXPECT_NEAR(1.0f, d.colour.z, kEPS);
//
//        EXPECT_EQ(std::string("[Kr] 4d10 5s2 5p6"), d.electronConfiguration.display);
//        EXPECT_EQ(std::string("Kr"), bracket_core(d.electronConfiguration.nobleGas));
//
//        expect_opt_float_near_optopt(d.physicalProperties.density.value,
//            std::optional<float>(0.0059f),
//            kEPS, "Density.Value");
//        EXPECT_EQ("g/cm^3", d.physicalProperties.density.unit);
//
//        const json j2 = cfg.toJson();
//        ElementConfig cfg2; ASSERT_NO_THROW(cfg2.fromJson(j2));
//        EXPECT_EQ(j2, cfg2.toJson());
//
//        ElementData d2 = cfg2.toData();
//        EXPECT_EQ(d.symbol, d2.symbol);
//        EXPECT_EQ(d.atomicNumber, d2.atomicNumber);
//        EXPECT_NEAR(d.atomicMass, d2.atomicMass, kEPS);
//    }
//
//} // namespace
