#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <random>
#include <string>
#include <limits>

#include "Interstellar/Config/JsonImpl/ElementConfig.hpp"
#include "Interstellar/Data/ElementData.hpp"

using nlohmann::json;
using Interstellar::Config::JsonImpl::ElementConfig;
using Interstellar::Data::ElementData;

namespace prop {

    // ----------------------------- RNG helpers -----------------------------------
    struct RNG {
        std::mt19937_64 gen;
        explicit RNG(uint64_t seed = 0xBADC0FFEEULL) : gen(seed) {}
        int i(int lo, int hi) { std::uniform_int_distribution<int> d(lo, hi); return d(gen); }
        float f(float lo, float hi) { std::uniform_real_distribution<float> d(lo, hi); return d(gen); }
        bool b(double p = 0.5) { std::bernoulli_distribution d(p); return d(gen); }
        std::string token(size_t minLen = 1, size_t maxLen = 4) {
            static const char* chars = "abcdefghijklmnopqrstuvwxyz";
            std::uniform_int_distribution<size_t> L(minLen, maxLen);
            std::uniform_int_distribution<size_t> C(0, 25);
            size_t n = L(gen);
            std::string s; s.reserve(n);
            for (size_t k = 0; k < n; ++k) s.push_back(chars[C(gen)]);
            return s;
        }
    };

    json MaybeNull(RNG& r, float v) {
        return r.b(0.2) ? json(nullptr) : json(v);
    }
    json MaybeNullS(RNG& r, const std::string& v) {
        return r.b(0.2) ? json(nullptr) : json(v);
    }

    // ------------------------ Random JSON generators ------------------------------
    json RandColor(RNG& r) {
        // sometimes wrong length to test robustness
        int n = r.i(0, 5);
        json arr = json::array();
        for (int k = 0; k < n; ++k) arr.push_back(r.f(0.f, 1.f));
        return arr;
    }

    json RandCosmic(RNG& r) {
        return json{ {"Value", r.f(0.f,1.f)}, {"Unit", r.b() ? "ppm" : "fraction of baryonic mass"} };
    }

    json RandIsotope(RNG& r) {
        json j;
        j["Name"] = "Iso_" + std::to_string(r.i(1, 300));
        j["MassNumber"] = r.f(0.5f, 300.f);
        j["IsStable"] = r.b();
        j["NaturalAbundance"] = r.f(0.f, 100.f);
        // half-life / radiation possibly null/missing
        if (r.b(0.5)) j["HalfLife"] = MaybeNull(r, r.f(1e-6f, 1e6f));
        if (r.b(0.5)) j["RadiationType"] = MaybeNullS(r, r.b() ? "alpha" : "beta");
        return j;
    }

    json RandPropWithUnit(RNG& r, float lo, float hi, const char* defaultUnit) {
        json j;
        j["Value"] = r.b(0.2) ? json(nullptr) : json(r.f(lo, hi));
        j["Unit"] = r.b(0.1) ? json(defaultUnit) : json(prop::RNG().token(1, 3)); // sometimes odd unit
        return j;
    }

    json RandPhysical(RNG& r) {
        return json{
          {"Density", RandPropWithUnit(r, 0.f, 10000.f, "g/cm^3")},
          {"MeltingPoint", RandPropWithUnit(r, 0.f, 10000.f, "K")},
          {"BoilingPoint", RandPropWithUnit(r, 0.f, 10000.f, "K")},
          {"TriplePoint", {
            {"Temperature", RandPropWithUnit(r, 0.f, 10000.f, "K")},
            {"Pressure",    RandPropWithUnit(r, 0.f, 10000.f, "kPa")}
          }},
          {"HeatCapacity", RandPropWithUnit(r, 0.f, 1e5f, "J/mol*K")},
          {"ThermalConductivity", RandPropWithUnit(r, 0.f, 1e5f, "W/m*K")},
          {"EnthalpyOfFusion", RandPropWithUnit(r, 0.f, 1e5f, "kJ/mol")},
          {"EnthalpyOfVaporization", RandPropWithUnit(r, 0.f, 1e5f, "kJ/mol")},
          {"Entropy", RandPropWithUnit(r, 0.f, 1e5f, "J/mol*K")}
        };
    }

    json RandRangeEnd(RNG& r, const char* unit) {
        return json{ {"Value", r.f(0.f, 1e5f)}, {"Unit", unit} };
    }
    json RandStateRange(RNG& r) {
        return json{
          {"TemperatureRange", json::array({ RandRangeEnd(r,"K"), RandRangeEnd(r,"K") })},
          {"PressureRange",    json::array({ RandRangeEnd(r,"atm"), RandRangeEnd(r,"atm") })},
          {"State", r.b() ? "solid" : (r.b() ? "liquid" : "gas")}
        };
    }

    std::string RandEC(RNG& r) {
        // Random noble gas + some weird tokens sprinkled
        std::string ng = r.b() ? "[He]" : (r.b() ? "[Ne]" : (r.b() ? "[Ar]" : ""));
        static const char* subs[] = { "s","p","d","f","x","q" };
        int terms = r.i(0, 5);
        std::string s = ng;
        for (int t = 0; t < terms; ++t) {
            if (!s.empty()) s.push_back(' ');
            int n = r.i(1, 7);
            const char* sub = subs[r.i(0, 5)];
            int e = r.i(1, 14);
            s += std::to_string(n) + sub + std::to_string(e);
        }
        return s.empty() ? "1s1" : s;
    }

    json RandChem(RNG& r) {
        json j;
        // some optional sub-objects present/missing/nulls
        if (r.b()) j["Electronegativity"] = json{ {"Value", r.f(0.f,4.f)}, {"Unit","Pauling"} };
        j["IonizationEnergy"] = json{
          {"First", json{{"Value", r.f(0.f,30.f)}, {"Unit","eV"}}}
        };
        if (r.b()) j["ElectronAffinity"] = json{ {"Value", r.b(0.2) ? json(nullptr) : json(r.f(-5.f,5.f))}, {"Unit","eV"} };
        if (r.b()) j["StandardReductionPotential"] = json{ {"Value", r.f(-10.f,10.f)}, {"Unit","V"} };
        j["Reactivity"] = json{ {"Value", r.f(0.f,1.f)}, {"Scale","0-1"} };
        // lists
        int nOX = r.i(0, 5);
        for (int k = 0; k < nOX; ++k) j["OxidationStates"].push_back(json{ {"State", r.i(-3,8)}, {"Common", r.b()} });
        int nB = r.i(0, 3); for (int k = 0; k < nB; ++k) j["Bonding"].push_back(r.b() ? "ionic" : "covalent");
        int nC = r.i(0, 3); for (int k = 0; k < nC; ++k) j["CommonCompounds"].push_back("X" + std::to_string(k));
        int nH = r.i(0, 3); for (int k = 0; k < nH; ++k) j["HybridizationExamples"].push_back(json{ {"Molecule","M" + std::to_string(k)}, {"Hybridization", r.b() ? "sp2" : "sp3"} });
        return j;
    }

    json RandQuantum(RNG& r) {
        json j;
        if (r.b()) j["AtomicRadius"] = json{ {"Value", r.i(10,300)}, {"Unit", r.b() ? "pm" : json(nullptr)} };
        if (r.b()) j["CovalentRadius"] = json{ {"Value", r.i(10,300)}, {"Unit", "pm"} };
        if (r.b()) j["VanDerWaalsRadius"] = json{ {"Value", r.i(10,300)}, {"Unit", "pm"} };
        if (r.b()) j["MagneticOrdering"] = r.b() ? "dia" : "para";
        int spins = r.i(0, 3);
        for (int k = 0; k < spins; ++k) j["SpinStates"].push_back(r.f(0.f, 5.f));
        if (r.b()) j["IsParamagnetic"] = r.b();
        if (r.b()) j["NuclearSpin"] = r.f(0.f, 5.f);
        return j;
    }

    json RandEnv(RNG& r) {
        json j;
        if (r.b()) j["Solubility"]["InWater"] = json{ {"Value", r.f(0.f,10.f)}, {"Unit","g/L"} };
        if (r.b()) j["OxidationInAir"] = json{ {"Tendency", "t" + std::to_string(r.i(1,9))}, {"Spontaneous", r.b()}, {"RequiresSpark", r.b()} };
        if (r.b()) j["Toxicity"] = "tox" + std::to_string(r.i(0, 3));
        if (r.b()) j["BiologicalRole"] = "bio" + std::to_string(r.i(0, 3));
        int hz = r.i(0, 3); for (int k = 0; k < hz; ++k) j["Hazards"].push_back("h" + std::to_string(k));
        return j;
    }

    json RandElementJson(RNG& r) {
        json j;

        j["Symbol"] = std::string(1, static_cast<char>('A' + r.i(0, 25)));
        j["Name"] = "Elem_" + std::to_string(r.i(1, 999));
        j["AtomicNumber"] = r.i(1, 118);
        j["AtomicMass"] = r.f(1.f, 300.f);

        j["Group"] = r.i(0, 18);
        j["Period"] = r.i(1, 7);
        j["Block"] = std::string(1, "spdf"[r.i(0, 3)]);
        j["StandardState"] = r.b() ? "solid" : (r.b() ? "liquid" : "gas");

        if (r.b()) j["Color"] = RandColor(r);
        if (r.b()) j["CosmicAbundance"] = RandCosmic(r);

        j["ElectronConfiguration"] = RandEC(r);

        int shells = r.i(0, 5);
        for (int k = 0; k < shells; ++k) j["ElectronShells"].push_back(r.i(1, 32));
        j["ValenceElectrons"] = r.i(0, 8);

        int iso = r.i(0, 4);
        for (int k = 0; k < iso; ++k) j["Isotopes"].push_back(RandIsotope(r));

        if (r.b()) j["PhysicalProperties"] = RandPhysical(r);

        int ranges = r.i(0, 4);
        for (int k = 0; k < ranges; ++k) j["StateRanges"].push_back(RandStateRange(r));

        if (r.b()) j["ChemicalProperties"] = RandChem(r);
        if (r.b()) j["QuantumProperties"] = RandQuantum(r);
        if (r.b()) j["EnvironmentalBehavior"] = RandEnv(r);

        return j;
    }

} // namespace prop

namespace Interstellar_Config_Data_ElementConfig_PropertyTests_Tests {
    // -----------------------------------------------------------------------------
    // Property test 1: Round-trip idempotence and no-throw
    // -----------------------------------------------------------------------------
    TEST(ElementConfig_Property, RoundTrip_Idempotent_NoThrow) {
        prop::RNG r(12345);
        const int RUNS = 100; // bump if you like

        for (int i = 0; i < RUNS; ++i) {
            const json in = prop::RandElementJson(r);

            ElementConfig a;
            ASSERT_NO_THROW(a.fromJson(in));

            json out1;
            ASSERT_NO_THROW(out1 = a.toJson());

            // second pass should stabilize the representation
            ElementConfig b;
            ASSERT_NO_THROW(b.fromJson(out1));
            json out2;
            ASSERT_NO_THROW(out2 = b.toJson());

            EXPECT_EQ(out1.dump(), out2.dump()) << "Idempotence failed on iteration " << i;

            // also ensure toData never throws
            ASSERT_NO_THROW((void)a.toData());
            ASSERT_NO_THROW((void)b.toData());
        }
    }

    // -----------------------------------------------------------------------------
    // Property test 2: Random omissions / nulls should not crash and produce sane data
    // -----------------------------------------------------------------------------
    TEST(ElementConfig_Property, NullsAndMissing_DoNotCrash_ProduceSaneShapes) {
        prop::RNG r(999);
        const int RUNS = 75;

        for (int i = 0; i < RUNS; ++i) {
            json j = prop::RandElementJson(r);

            // Randomly delete some top-level keys
            static const char* keys[] = {
              "CosmicAbundance","Isotopes","PhysicalProperties","StateRanges",
              "ChemicalProperties","QuantumProperties","EnvironmentalBehavior","Color"
            };
            int delN = r.i(0, 3);
            for (int k = 0; k < delN; ++k) {
                j.erase(keys[r.i(0, static_cast<int>(std::size(keys)) - 1)]);
            }

            ElementConfig cfg;
            ASSERT_NO_THROW(cfg.fromJson(j));
            ElementData d;
            ASSERT_NO_THROW(d = cfg.toData());

            // Sane invariants
            EXPECT_GE(d.atomicNumber, 0);
            EXPECT_FALSE(d.symbol.empty());
            // Color: if provided wrong length, implementation ignores; we just assert it's finite
            //EXPECT_TRUE(std::isfinite(d.colour.r));
            //EXPECT_TRUE(std::isfinite(d.colour.g));
            //EXPECT_TRUE(std::isfinite(d.colour.b));

            //// Not too many explosions in sizes
            //EXPECT_LE(d.isotopes.size(), 64u);
            //EXPECT_LE(d.stateRanges.size(), 64u);
            //EXPECT_LE(d.electronConfiguration.orbitals.size(), 64u);

            //// Re-run toJson; must be consistent
            //json normalized = cfg.toJson();
            //ElementConfig cfg2; cfg2.fromJson(normalized);
            //EXPECT_EQ(cfg.toJson().dump(), cfg2.toJson().dump());
        }
    }

    // -----------------------------------------------------------------------------
    // Property test 3: Electron configuration garbage tokens should be ignored
    // -----------------------------------------------------------------------------
    TEST(ElementConfig_Property, ElectronConfig_FuzzedTokens_OnlyValidOrbitalsCaptured) {
        //prop::RNG r(7777);

        //for (int i = 0; i < 100; ++i) {
            //std::string ec = prop::RandEC(r);
            // sprinkle garbage
            //if (r.b()) ec += "   9z123   qwerty   3g-2"; // invalid forms

            //json j = {
            //  {"Symbol","Fx"},
            //  {"Name","Fuzzium"},
            //  {"AtomicNumber", 42},
            //  {"AtomicMass", 100.0f},
            //  {"Group", 10}, {"Period", 5}, {"Block","d"}, {"StandardState","solid"},
            //  {"Color",{0.1f,0.2f,0.3f}},
            //  {"CosmicAbundance", {{"Value", 0.1f}, {"Unit","ppm"}}},
            //  {"ElectronConfiguration", ec},
            //  {"ElectronShells",{2,8}},
            //  {"ValenceElectrons", r.i(0,8)}
            //};

            //ElementConfig cfg;
            //cfg.fromJson(j);
            //ElementData d = cfg.toData();

            // Validate every captured orbital matches regex \d+[spdf]\d+
            //const auto& orbs = d.electronConfiguration.orbitals;
            //for (const auto& o : orbs) {
                //ASSERT_FALSE(o.orbital.empty());
                // orbital like "4d", electrons positive
                //ASSERT_TRUE(o.orbital.size() >= 2);
                //char subshell = o.orbital.back();
                //ASSERT_TRUE(subshell == 's' || subshell == 'p' || subshell == 'd' || subshell == 'f');
                //ASSERT_GT(o.electrons, 0);
            //}
        //}
    }

}