#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "nlohmann/json.hpp"

#include "Interstellar/Config/JsonImpl/ElementConfig.hpp"
#include "Interstellar/Data/ElementData.hpp"

#include "Interstellar/Utils/Json/JsonHelpers.hpp"
#include "Interstellar/Utils/Time/TimeUtils.hpp"
#include "TestSupport.hpp"

namespace Interstellar_Config_JsonImpl_ElementConfig_DeterministicTests_Test {
    
    using nlohmann::json;
    using Interstellar::Config::JsonImpl::ElementConfig;
    using Interstellar::Data::ElementData;
    using namespace Interstellar::Utils::Json;
    using Interstellar::Utils::Time::string_to_time_unit;
    using Interstellar::Utils::Time::to_seconds;
    using namespace TestSupport;

    // Whole file canonicalization stability: if we canonicalize the entire array,
    // re-reading that array remains byte-for-byte equal.
    TEST(ElementConfig_AllElements, WholeFile_CanonicalArray_Idempotent) {
        const auto text = read_text_file(TEST_ELEMENTS_JSON);
        json doc = json::parse(text);
        ASSERT_TRUE(doc.is_array());

        // Build canonical array once
        json canon = json::array();
        for (const auto& elem : doc) {
            ElementConfig cfg;
            cfg.fromJson(elem);
            canon.push_back(cfg.toJson());
        }

        //// Re-read canonical array and confirm stable
        json canon_again = json::array();
        for (const auto& elem : canon) {
            ElementConfig cfg;
            cfg.fromJson(elem);
            canon_again.push_back(cfg.toJson());
        }

        EXPECT_EQ(canon, canon_again);
    }


    // Element-level smoke: JSON -> ElementConfig -> JSON is idempotent (canonical form),
    // and JSON -> ElementConfig -> ElementData does not throw.
    TEST(ElementConfig_AllElements, PerElement_Idempotent_And_Mappable) {
        const auto text = read_text_file(TEST_ELEMENTS_JSON);
        json doc = json::parse(text);
        ASSERT_TRUE(doc.is_array()) << "elements_all.json must be a JSON array";

        for (size_t i = 0; i < doc.size(); ++i) {
            const json& src = doc[i];
            SCOPED_TRACE(std::string("element #") + std::to_string(i) + " symbol=" + src.value("Symbol", "<none>"));

            // Read
            ElementConfig cfg;
            ASSERT_NO_THROW(cfg.fromJson(src));

            // Map to runtime (just to ensure mapping paths don't throw)
            EXPECT_NO_THROW({
                ElementData d = cfg.toData();
                (void)d;
                });

            // Canonicalize
            const json canon = cfg.toJson();

            // Re-read canonical form and ensure it's stable
            ElementConfig cfg2;
            ASSERT_NO_THROW(cfg2.fromJson(canon));
            const json canon2 = cfg2.toJson();
            EXPECT_EQ(canon, canon2);
        }
    }

    TEST(ElementConfig_AllElements, FromJson_ToData_And_RoundTrip_Are_Correct) {
        // Load elements file once
        const auto text = read_text_file(TEST_ELEMENTS_JSON);
        json doc = json::parse(text);
        ASSERT_TRUE(doc.is_array()) << "elements_all.json must be a JSON array";

        for (size_t i = 0; i < doc.size(); ++i) {
            const json& J = doc[i];
            SCOPED_TRACE(std::string("idx: ") + std::to_string(i) +
                " symbol: " + J.value("Symbol", std::string("<none>")));

            ElementConfig cfg;
            ASSERT_NO_THROW(cfg.fromJson(J));
            ElementData data = cfg.toData();

            // Identity / periodic
            EXPECT_EQ(J.at("Symbol").get<std::string>(), data.symbol);
            EXPECT_EQ(J.at("Name").get<std::string>(), data.name);
            EXPECT_EQ(get_json_to_int(J, "AtomicNumber"), data.atomicNumber);
            EXPECT_NEAR(get_json_to_float(J, "AtomicMass"), data.atomicMass, kEPS);

            auto j_group = get_json_to_opt_int(J, "Group");

            // Primary: parsed numeric group must match.
            EXPECT_EQ(j_group, data.group) << "element #" << i << " symbol: " << data.symbol;

            // Schema-aware assertions
            if (!J.contains("Group")) {
                EXPECT_FALSE(data.group) << "element #" << i << " should have no group when JSON key is missing";
            }
            else if (J["Group"].is_null()) {
                EXPECT_FALSE(data.group) << "element #" << i << " should have no group when JSON is null";
            }
            else if (J["Group"].is_number()) {
                ASSERT_TRUE(data.group) << "element #" << i << " numeric Group must be present in data";
                // value equality already covered by EXPECT_EQ above
            }
            else if (J["Group"].is_string()) {
                // f-block labels etc.
                EXPECT_FALSE(data.group) << "element #" << i << " string Group should not produce a numeric group";
                // (optional) also verify series mapping if you store it:
                 EXPECT_TRUE(data.series);
                 EXPECT_EQ(Interstellar::Data::string_to_element_series(J["Group"].get<std::string>()), data.series.value());
            }
            else {
                // Any other JSON type treat as "no numeric group"
                EXPECT_FALSE(data.group);
            }

            if (J.contains("Period")) EXPECT_EQ(get_json_to_int(J,"Period"), data.period);
            if (J.contains("Block"))  EXPECT_EQ(get_json_to_string(J, "Block"), data.block);
            if (J.contains("StandardState")) EXPECT_EQ(get_json_to_string(J,"StandardState"), data.standardState);

            if (J.contains("Color")) {
                auto col = J["Color"].get<std::vector<float>>();
                ASSERT_EQ(col.size(), 4u);
                EXPECT_NEAR(col[0], data.colour.x, kEPS);
                EXPECT_NEAR(col[1], data.colour.y, kEPS);
                EXPECT_NEAR(col[2], data.colour.z, kEPS);
                EXPECT_NEAR(col[3], data.colour.a, kEPS);
            }

            if (J.contains("CosmicAbundance")) {
                const auto& ca = J["CosmicAbundance"];
                ASSERT_TRUE(ca.is_object()) << "CosmicAbundance must be an object";

                // Value (optional number or null)
                std::optional<float> exp;
                if (ca.contains("Value") && !ca["Value"].is_null()) {
                    ASSERT_TRUE(ca["Value"].is_number()) << "CosmicAbundance.Value must be number or null";
                    exp = ca["Value"].get<float>();
                }
                expect_opt_float_near_optopt(data.cosmicAbundance.value, exp, kEPS, "CosmicAbundance.Value");

                // Unit (optional string)
                if (ca.contains("Unit") && !ca["Unit"].is_null()) {
                    ASSERT_TRUE(ca["Unit"].is_string()) << "CosmicAbundance.Unit must be string or null";
                    EXPECT_EQ(ca["Unit"].get<std::string>(), data.cosmicAbundance.unit)
                        << "CosmicAbundance.Unit mismatch";
                }
                else {
                    // If JSON omits Unit or sets it null, runtime should have empty unit (or whatever default you chose)
                    EXPECT_TRUE(data.cosmicAbundance.unit.empty())
                        << "Unit missing/null in JSON but runtime unit is non-empty";
                }
            }
            else {
                // Entire block missing -> runtime must reflect "no data"
                EXPECT_FALSE(data.cosmicAbundance.value.has_value())
                    << "CosmicAbundance missing in JSON but runtime has a value";
                EXPECT_TRUE(data.cosmicAbundance.unit.empty())
                    << "CosmicAbundance missing in JSON but runtime unit is non-empty";
            }

            // Electron configuration
            if (J.contains("ElectronConfiguration")) {
                ASSERT_TRUE(J["ElectronConfiguration"].is_string())
                    << "ElectronConfiguration must be a string for element #" << i << " (" << data.symbol << ")";
                const auto disp = J["ElectronConfiguration"].get<std::string>();

                // Round-trip display string
                EXPECT_EQ(disp, data.electronConfiguration.display)
                    << "Display mismatch for element #" << i << " (" << data.symbol << ")";

                // Noble gas core (without brackets)
                std::string expectedCore;
                if (!disp.empty() && disp.front() == '[') {
                    const auto end = disp.find(']');
                    if (end != std::string::npos) {
                        expectedCore = extractBracketContent(disp.substr(0, end + 1));
                    }
                }
                EXPECT_EQ(expectedCore, data.electronConfiguration.nobleGas)
                    << "Noble gas core mismatch for element #" << i << " (" << data.symbol << ")";

                // Parse from the display string
                ElementConfig tmp;
                const auto parsed = tmp.parseElectronConfiguration(disp);

                // Basic size check
                ASSERT_EQ(parsed.orbitals.size(), data.electronConfiguration.orbitals.size())
                    << "Orbitals size mismatch for element #" << i << " (" << data.symbol << ")";

                // Build a lookup from runtime data
                std::unordered_map<std::string, int> runtime;
                runtime.reserve(data.electronConfiguration.orbitals.size());
                for (const auto& o : data.electronConfiguration.orbitals) {
                    EXPECT_TRUE(runtime.emplace(o.orbital, o.electrons).second)
                        << "Duplicate orbital '" << o.orbital << "' in runtime for element #" << i
                        << " (" << data.symbol << ")";
                }

                // Compare parsed against runtime (order-insensitive)
                for (const auto& orbit : parsed.orbitals) {
                    EXPECT_GE(orbit.electrons, 0)
                        << "Electron count must be non-negative for '" << orbit.orbital
                        << "' in element #" << i << " (" << data.symbol << ")";

                    auto it = runtime.find(orbit.orbital);
                    ASSERT_NE(it, runtime.end())
                        << "Orbital '" << orbit.orbital << "' not found in runtime for element #"
                        << i << " (" << data.symbol << ")";
                    EXPECT_EQ(it->second, orbit.electrons)
                        << "Electron count mismatch for orbital '" << orbit.orbital
                        << "' in element #" << i << " (" << data.symbol << ")";
                }

                int total = 0;
                for (const auto& o : parsed.orbitals) total += o.electrons;
                EXPECT_LE(total, data.atomicNumber)
                    << "Parsed electrons exceed Z for element #" << i << " (" << data.symbol << ")";
            }
            else {
                // Entire field missing runtime should be empty/cleared
                EXPECT_TRUE(data.electronConfiguration.orbitals.empty())
                    << "ElectronConfiguration expected to have no orbitals for element #" << i
                    << " (" << data.symbol << ")";
                EXPECT_TRUE(data.electronConfiguration.nobleGas.empty())
                    << "ElectronConfiguration expected to have no noble gas for element #" << i
                    << " (" << data.symbol << ")";
                EXPECT_TRUE(data.electronConfiguration.display.empty())
                    << "ElectronConfiguration expected to have no display for element #" << i
                    << " (" << data.symbol << ")";
            }

            // Shells / valence
            if (J.contains("ElectronShells") && J["ElectronShells"].is_array()) {
                auto shells = J["ElectronShells"].get<std::vector<int>>();
                ASSERT_EQ(shells.size(), data.electronShells.size());
                for (size_t k = 0; k < shells.size(); ++k)
                    EXPECT_EQ(shells[k], data.electronShells[k]);
            }

            if (J.contains("ValenceElectrons"))
                EXPECT_EQ(J["ValenceElectrons"].get<int>(), data.valenceElectrons);

            // Isotopes
            if (J.contains("Isotopes")) {
                ASSERT_TRUE(J["Isotopes"].is_array()) << "Isotopes must be an array";
                const auto& isotopeArray = J["Isotopes"];
                ASSERT_EQ(isotopeArray.size(), data.isotopes.size())
                    << "isotopes size mismatch for " << data.symbol;

                for (size_t k = 0; k < isotopeArray.size(); ++k) {
                    const auto& ji = isotopeArray[k];
                    const auto& di = data.isotopes[k];

                    SCOPED_TRACE(testing::Message()
                        << "element " << data.symbol << " isotope index " << k);

                    // Name
                    if (ji.contains("Name")) {
                        ASSERT_TRUE(ji["Name"].is_string());
                        EXPECT_EQ(ji["Name"].get<std::string>(), di.name);
                    }

                    // MassNumber
                    if (ji.contains("MassNumber")) {
                        ASSERT_TRUE(ji["MassNumber"].is_number());
                        EXPECT_NEAR(ji["MassNumber"].get<float>(), di.massNumber, kEPS);
                    }

                    // IsStable
                    if (ji.contains("IsStable")) {
                        ASSERT_TRUE(ji["IsStable"].is_boolean())
                            << "IsStable must be a boolean for isotope (" << data.symbol << ", " << k << ")";

                        auto isstable = ji.value("IsStable", true);
                        //EXPECT_EQ(ji["IsStable"].template get<bool>(), di.isStable);
                    }

                    // NaturalAbundance
                    if (ji.contains("NaturalAbundance")) {
                        if (ji["NaturalAbundance"].is_null()) {
                            EXPECT_NEAR(di.naturalAbundance, 0.0f, kEPS);
                        }
                        else {
                            ASSERT_TRUE(ji["NaturalAbundance"].is_number());
                            EXPECT_NEAR(ji["NaturalAbundance"].get<float>(), di.naturalAbundance, kEPS);
                        }
                    }

                    // HalfLife: null or {Value:number, Unit:string}
                    if (ji.contains("HalfLife")) {
                        const auto& hl = ji["HalfLife"];
                        if (hl.is_null()) {
                            EXPECT_FALSE(di.halfLife.has_value()) << "halfLife should be empty";
                        }
                        else {
                            ASSERT_TRUE(hl.is_object())
                                << "HalfLife must be an object with {Value, Unit} or null";
                            ASSERT_TRUE(hl.contains("Value") && hl["Value"].is_number())
                                << "HalfLife.Value must be numeric";
                            ASSERT_TRUE(hl.contains("Unit") && hl["Unit"].is_string())
                                << "HalfLife.Unit must be a string";

                            const double v = hl["Value"].get<double>();
                            const std::string u = hl["Unit"].get<std::string>();

                            // Compare fields
                            EXPECT_DOUBLE_EQ(di.halfLife->value, v);
                            EXPECT_EQ(di.halfLife->unit, string_to_time_unit(u));

                            // Optional sanity: compare normalized seconds
                            const double expectedSec = to_seconds(v, u);
                            const double gotSec = to_seconds(di.halfLife->value, di.halfLife->unit);
                            EXPECT_NEAR(gotSec, expectedSec, 1e-9);
                        }
                    }
                    else {
                        // Not present -> empty optional (if that's your contract)
                        EXPECT_FALSE(di.halfLife.has_value());
                    }

                    // RadiationType: null or string
                    if (ji.contains("RadiationType")) {
                        const auto& rt = ji["RadiationType"];
                        if (rt.is_null()) {
                            EXPECT_FALSE(di.radiationType.has_value());
                        }
                        else {
                            ASSERT_TRUE(rt.is_string());
                            ASSERT_TRUE(di.radiationType.has_value());
                            EXPECT_EQ(rt.get<std::string>(), di.radiationType.value());
                        }
                    }
                    else {
                        EXPECT_FALSE(di.radiationType.has_value());
                    }
                }
            }

            // Physical properties
            //if (J.contains("PhysicalProperties")) {
                //const auto& P = J["PhysicalProperties"];
                //auto checkProp = [&](const char* name,
                    //const std::optional<float>& gotVal,
                    //const std::string& gotUnit) {
                        //if (!P.contains(name)) return;
                        //const auto& node = P[name];
                        //std::optional<float> expVal;
                        //std::string expUnit;
                        //if (node.contains("Value") && !node["Value"].is_null())
                            //expVal = node["Value"].get<float>();
                        //if (node.contains("Unit") && node["Unit"].is_string())
                            //expUnit = node["Unit"].get<std::string>();
                        //expect_opt_float_near_optopt(gotVal, expVal, kEPS, name);
                        //if (!expUnit.empty()) EXPECT_EQ(expUnit, gotUnit) << name;
                    //};

                //checkProp("Density", d.physicalProperties.density.value, d.physicalProperties.density.unit);
                //checkProp("MeltingPoint", d.physicalProperties.meltingPoint.value, d.physicalProperties.meltingPoint.unit);
                //checkProp("BoilingPoint", d.physicalProperties.boilingPoint.value, d.physicalProperties.boilingPoint.unit);
                //checkProp("HeatCapacity", d.physicalProperties.heatCapacity.value, d.physicalProperties.heatCapacity.unit);
                //checkProp("ThermalConductivity", d.physicalProperties.thermalConductivity.value, d.physicalProperties.thermalConductivity.unit);
                //checkProp("EnthalpyOfFusion", d.physicalProperties.enthalpyOfFusion.value, d.physicalProperties.enthalpyOfFusion.unit);
                //checkProp("EnthalpyOfVaporization", d.physicalProperties.enthalpyOfVaporization.value, d.physicalProperties.enthalpyOfVaporization.unit);
                //checkProp("Entropy", d.physicalProperties.entropy.value, d.physicalProperties.entropy.unit);

                //if (P.contains("TriplePoint")) {
                    //const auto& TP = P["TriplePoint"];
                    //auto chk = [](const nlohmann::json& n,
                        //const std::optional<float>& gv, const std::string& gu,
                        //const char* nm) {
                            //std::optional<float> ev;
                            //std::string eu;
                            //if (n.contains("Value") && !n["Value"].is_null()) ev = n["Value"].get<float>();
                            //if (n.contains("Unit") && n["Unit"].is_string())  eu = n["Unit"].get<std::string>();
                            //expect_opt_float_near_optopt(gv, ev, kEPS, nm);
                            //if (!eu.empty()) EXPECT_EQ(eu, gu) << nm;
                        //};
                    //if (TP.contains("Temperature"))
                        //chk(TP["Temperature"], d.physicalProperties.triplePoint.temperature.value,
                            //d.physicalProperties.triplePoint.temperature.unit, "TriplePoint.Temperature");
                    //if (TP.contains("Pressure"))
                        //chk(TP["Pressure"], d.physicalProperties.triplePoint.pressure.value,
                            //d.physicalProperties.triplePoint.pressure.unit, "TriplePoint.Pressure");
                //}
            //}

            // State ranges
            //if (J.contains("StateRanges") && J["StateRanges"].is_array()) {
                //const auto& R = J["StateRanges"];
                //ASSERT_EQ(R.size(), d.stateRanges.size());
                //for (size_t k = 0; k < R.size(); ++k) {
                    //const auto& rj = R[k];
                    //const auto& rd = d.stateRanges[k];

                    //auto chkRange = [&](const nlohmann::json& arr,
                        //const std::optional<float>& minV, const std::string& minU,
                        //const std::optional<float>& maxV, const std::string& maxU,
                        //const char* nm) {
                            //ASSERT_TRUE(arr.is_array());
                            //ASSERT_EQ(arr.size(), 2u);
                            //std::optional<float> vmin, vmax;
                            //std::string umin, umax;
                            //if (arr[0].contains("Value") && arr[0]["Value"].is_number())
                                //vmin = arr[0]["Value"].get<float>();
                            //if (arr[0].contains("Unit") && arr[0]["Unit"].is_string())
                                //umin = arr[0]["Unit"].get<std::string>();
                            //if (arr[1].contains("Value") && arr[1]["Value"].is_number())
                                //vmax = arr[1]["Value"].get<float>();
                            //if (arr[1].contains("Unit") && arr[1]["Unit"].is_string())
                                //umax = arr[1]["Unit"].get<std::string>();

                            //expect_opt_float_near_optopt(minV, vmin, kEPS, (std::string(nm) + ".min").c_str());
                            //expect_opt_float_near_optopt(maxV, vmax, kEPS, (std::string(nm) + ".max").c_str());
                            //if (!umin.empty()) EXPECT_EQ(umin, minU) << nm << ".min.unit";
                            //if (!umax.empty()) EXPECT_EQ(umax, maxU) << nm << ".max.unit";
                        //};

                    //if (rj.contains("TemperatureRange"))
                        //chkRange(rj["TemperatureRange"], rd.temperature.min.value, rd.temperature.min.unit,
                            //rd.temperature.max.value, rd.temperature.max.unit, "TemperatureRange");

                    //if (rj.contains("PressureRange"))
                        //chkRange(rj["PressureRange"], rd.pressure.min.value, rd.pressure.min.unit,
                            //rd.pressure.max.value, rd.pressure.max.unit, "PressureRange");

                    //if (rj.contains("State") && rj["State"].is_string())
                        //EXPECT_EQ(rj["State"].get<std::string>(), rd.state);
                //}
            //}

            // Chemical
            //if (J.contains("ChemicalProperties")) {
                //const auto& C = J["ChemicalProperties"];

                //if (C.contains("Electronegativity")) {
                    //const auto& E = C["Electronegativity"];
                    //std::optional<float> ev;
                    //std::string eu;
                    //if (E.contains("Value") && !E["Value"].is_null()) ev = E["Value"].get<float>();
                    //if (E.contains("Unit") && E["Unit"].is_string())  eu = E["Unit"].get<std::string>();
                    //expect_opt_float_near_optopt(d.chemical.electronegativity.value, ev, kEPS, "Electronegativity.Value");
                    //if (!eu.empty()) EXPECT_EQ(eu, d.chemical.electronegativity.unit);
                //}

                //{
                    //auto [ev, eu] = extract_ionization_e(C);
                    //expect_opt_float_near_optopt(d.chemical.ionizationEnergy.first.value, ev, kEPS, "IonizationEnergy.First.Value");
                    //if (!eu.empty()) EXPECT_EQ(eu, d.chemical.ionizationEnergy.first.unit);
                //}

                //if (C.contains("ElectronAffinity")) {
                    //const auto& EA = C["ElectronAffinity"];
                    //std::optional<float> ev;
                    //std::string eu;
                    //if (EA.contains("Value") && !EA["Value"].is_null()) ev = EA["Value"].get<float>();
                    //if (EA.contains("Unit") && EA["Unit"].is_string())  eu = EA["Unit"].get<std::string>();
                    //expect_opt_float_near_optopt(d.chemical.electronAffinity.value, ev, kEPS, "ElectronAffinity.Value");
                    //if (!eu.empty()) EXPECT_EQ(eu, d.chemical.electronAffinity.unit);
                //}

                //if (C.contains("StandardReductionPotential")) {
                    //const auto& RP = C["StandardReductionPotential"];
                    //std::optional<float> ev;
                    //std::string eu;
                    //if (RP.contains("Value") && !RP["Value"].is_null()) ev = RP["Value"].get<float>();
                    //if (RP.contains("Unit") && RP["Unit"].is_string())  eu = RP["Unit"].get<std::string>();
                    //expect_opt_float_near_optopt(d.chemical.standardReductionPotential.value, ev, kEPS, "StandardReductionPotential.Value");
                    //if (!eu.empty()) EXPECT_EQ(eu, d.chemical.standardReductionPotential.unit);
                //}

                //if (C.contains("Reactivity")) {
                    //const auto& R = C["Reactivity"];
                    //if (R.contains("Value") && R["Value"].is_number())
                        //EXPECT_NEAR(R["Value"].get<float>(), d.chemical.reactivity.value, kEPS);
                    //if (R.contains("Scale") && R["Scale"].is_string())
                        //EXPECT_EQ(R["Scale"].get<std::string>(), d.chemical.reactivity.scale);
                //}

                //if (C.contains("OxidationStates") && C["OxidationStates"].is_array()) {
                    //auto arr = C["OxidationStates"];
                    //ASSERT_EQ(arr.size(), d.chemical.oxidationStates.size());
                    //for (size_t k = 0; k < arr.size(); ++k) {
                        //if (arr[k].contains("State"))
                            //EXPECT_EQ(arr[k]["State"].get<int>(), d.chemical.oxidationStates[k].state);
                        //if (arr[k].contains("Common"))
                            //EXPECT_EQ(arr[k]["Common"].get<bool>(), d.chemical.oxidationStates[k].common);
                    //}
                //}

                //if (C.contains("Bonding") && C["Bonding"].is_array()) {
                    //auto vb = C["Bonding"].get<std::vector<std::string>>();
                    //ASSERT_EQ(vb.size(), d.chemical.bonding.size());
                    //for (size_t k = 0; k < vb.size(); ++k) EXPECT_EQ(vb[k], d.chemical.bonding[k]);
                //}

                //if (C.contains("CommonCompounds") && C["CommonCompounds"].is_array()) {
                    //auto cc = C["CommonCompounds"].get<std::vector<std::string>>();
                    //ASSERT_EQ(cc.size(), d.chemical.commonCompounds.size());
                    //for (size_t k = 0; k < cc.size(); ++k) EXPECT_EQ(cc[k], d.chemical.commonCompounds[k]);
                //}

                //if (C.contains("HybridizationExamples") && C["HybridizationExamples"].is_array()) {
                    //auto hx = C["HybridizationExamples"];
                    //ASSERT_EQ(hx.size(), d.chemical.hybridizationExamples.size());
                    //for (size_t k = 0; k < hx.size(); ++k) {
                        //const auto& jh = hx[k];
                        //const auto& dh = d.chemical.hybridizationExamples[k];
                        //if (jh.contains("Molecule"))      EXPECT_EQ(jh["Molecule"].get<std::string>(), dh.first);
                        //if (jh.contains("Hybridization"))  EXPECT_EQ(jh["Hybridization"].get<std::string>(), dh.second);
                    //}
                //}
            //}

            // Quantum (assert only fields your impl fills)
            //if (J.contains("QuantumProperties")) {
                //const auto& Q = J["QuantumProperties"];
                //auto chkRadius = [&](const char* name,
                    //const std::optional<int>& got,
                    //const std::optional<std::string>& unit) {
                        //if (!Q.contains(name)) return;
                        //const auto& node = Q[name];
                        //std::optional<int> ev;
                        //std::optional<std::string> eu;
                        //if (node.contains("Value") && node["Value"].is_number_integer())
                            //ev = node["Value"].get<int>();
                        //if (node.contains("Unit") && node["Unit"].is_string())
                            //eu = node["Unit"].get<std::string>();
                        //expect_opt_eq(got, ev, name);
                        //expect_opt_eq(unit, eu, (std::string(name) + ".Unit").c_str());
                    //};
                //chkRadius("AtomicRadius", d.quantum.atomicRadius.value, d.quantum.atomicRadius.unit);
                //chkRadius("CovalentRadius", d.quantum.covalentRadius.value, d.quantum.covalentRadius.unit);
                //chkRadius("VanDerWaalsRadius", d.quantum.vanDerWaalsRadius.value, d.quantum.vanDerWaalsRadius.unit);

                //if (Q.contains("MagneticOrdering") && Q["MagneticOrdering"].is_string())
                    //EXPECT_EQ(Q["MagneticOrdering"].get<std::string>(), d.quantum.magneticOrdering);
                //if (Q.contains("SpinStates") && Q["SpinStates"].is_array()) {
                    //const auto sj = Q["SpinStates"].get<std::vector<float>>();
                    //ASSERT_EQ(sj.size(), d.quantum.spinStates.size());
                    //for (size_t k = 0; k < sj.size(); ++k) EXPECT_NEAR(sj[k], d.quantum.spinStates[k], kEPS);
                //}
                //if (Q.contains("IsParamagnetic") && Q["IsParamagnetic"].is_boolean())
                    //EXPECT_EQ(Q["IsParamagnetic"].get<bool>(), d.quantum.isParamagnetic);
                //if (Q.contains("NuclearSpin") && Q["NuclearSpin"].is_number())
                    //EXPECT_NEAR(Q["NuclearSpin"].get<float>(), d.quantum.nuclearSpin, kEPS);
            //}

            // Environmental
            //if (J.contains("EnvironmentalBehavior")) {
                //const auto& E = J["EnvironmentalBehavior"];
                //if (E.contains("Solubility") && E["Solubility"].contains("InWater")) {
                    //const auto& W = E["Solubility"]["InWater"];
                    //std::optional<float> ev;
                    //std::optional<std::string> eu;
                    //if (W.contains("Value") && W["Value"].is_number()) ev = W["Value"].get<float>();
                    //if (W.contains("Unit") && W["Unit"].is_string())    eu = W["Unit"].get<std::string>();
                    //expect_opt_float_near_optopt(d.environmentalProp.solubilityInWater.value, ev, kEPS, "SolubilityInWater.Value");
                    //expect_opt_eq(d.environmentalProp.solubilityInWater.unit, eu, "SolubilityInWater.Unit");
                //}
                //if (E.contains("OxidationInAir")) {
                    //const auto& O = E["OxidationInAir"];
                    //if (O.contains("Tendency") && O["Tendency"].is_string())
                        //EXPECT_EQ(O["Tendency"].get<std::string>(), d.environmentalProp.oxidationInAir.tendency);
                    //if (O.contains("Spontaneous") && O["Spontaneous"].is_boolean())
                        //EXPECT_EQ(O["Spontaneous"].get<bool>(), d.environmentalProp.oxidationInAir.spontaneous);
                    //if (O.contains("RequiresSpark") && O["RequiresSpark"].is_boolean())
                        //EXPECT_EQ(O["RequiresSpark"].get<bool>(), d.environmentalProp.oxidationInAir.requiresSpark);
                //}
                //if (E.contains("Toxicity") && E["Toxicity"].is_string())
                    //EXPECT_EQ(E["Toxicity"].get<std::string>(), d.environmentalProp.toxicity);
                //if (E.contains("BiologicalRole") && E["BiologicalRole"].is_string())
                    //EXPECT_EQ(E["BiologicalRole"].get<std::string>(), d.environmentalProp.biologicalRole);
                //if (E.contains("Hazards") && E["Hazards"].is_array()) {
                    //auto hz = E["Hazards"].get<std::vector<std::string>>();
                    //ASSERT_EQ(hz.size(), d.environmentalProp.hazards.size());
                    //for (size_t k = 0; k < hz.size(); ++k) EXPECT_EQ(hz[k], d.environmentalProp.hazards[k]);
                //}
            //}

            // Round-trip idempotence
            //const json J2 = cfg.toJson();
            //ElementConfig cfg2; ASSERT_NO_THROW(cfg2.fromJson(J2));
            //const json J3 = cfg2.toJson();
            //EXPECT_EQ(J2, J3);

            // Mapping still consistent
            //ElementData d2 = cfg2.toData();
            //EXPECT_EQ(d.symbol, d2.symbol);
            //EXPECT_EQ(d.atomicNumber, d2.atomicNumber);
            //EXPECT_NEAR(d.atomicMass, d2.atomicMass, kEPS);
        }
    }

    //TEST(ElementConfig_ElectronConfig, Parses_Common_Patterns) {
        //ElementConfig cfg;
        //auto parsed = cfg.parseElectronConfiguration("[Kr] 4d10 5s2 5p6");
        //EXPECT_EQ("Kr", bracket_core(parsed.NobleGas));
        //ASSERT_EQ(parsed.Orbitals.size(), 3u);
        //EXPECT_EQ(parsed.Orbitals[0].Orbital, "4d");
        //EXPECT_EQ(parsed.Orbitals[0].Electrons, 10);
        //EXPECT_EQ(parsed.Orbitals[1].Orbital, "5s");
        //EXPECT_EQ(parsed.Orbitals[1].Electrons, 2);
        //EXPECT_EQ(parsed.Orbitals[2].Orbital, "5p");
        //EXPECT_EQ(parsed.Orbitals[2].Electrons, 6);

        //auto h = cfg.parseElectronConfiguration("1s1");
        //EXPECT_TRUE(h.NobleGas.empty());
        //ASSERT_EQ(h.Orbitals.size(), 1u);
        //EXPECT_EQ(h.Orbitals[0].Orbital, "1s");
        //EXPECT_EQ(h.Orbitals[0].Electrons, 1);
    //}

    //TEST(ElementConfig_ElectronConfig, Ignores_Unrecognized_Tokens) {
        //ElementConfig cfg;
        //auto parsed = cfg.parseElectronConfiguration("[Ne] 3s2 oops 3p6");
        //EXPECT_EQ("Ne", bracket_core(parsed.NobleGas));
        //ASSERT_EQ(parsed.Orbitals.size(), 2u);
        //EXPECT_EQ(parsed.Orbitals[0].Orbital, "3s");
        //EXPECT_EQ(parsed.Orbitals[0].Electrons, 2);
        //EXPECT_EQ(parsed.Orbitals[1].Orbital, "3p");
        //EXPECT_EQ(parsed.Orbitals[1].Electrons, 6);
    //}

} // namespace
