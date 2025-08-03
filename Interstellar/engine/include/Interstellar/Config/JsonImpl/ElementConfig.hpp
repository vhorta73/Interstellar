#pragma once

#include "Interstellar/Config/JsonConfigBase.hpp"
#include "Interstellar/Data/ElementData.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <regex>
#include <iostream>
#include "Interstellar/Utils/StringUtils.hpp"

#include "Interstellar/Core/Logging.hpp"

namespace Interstellar::Config {
    
    struct IntUnit {
        std::optional<int> Value;
        std::optional<std::string> Unit;
    };

    struct FloatUnit {
        std::optional<float> Value;
        std::optional<std::string> Unit;
    };

    struct ConfigCosmicAbundance {
        float Value = 0.0f;
        std::string Unit;
    };

    struct ConfigOrbital {
        std::string Orbital;
        int Electrons = 0;
    };

    struct ConfigElectronConfiguration {
        std::string NobleGas;
        std::string Display;
        std::vector<ConfigOrbital> Orbitals;
    };

    struct ConfigIsotope {
        std::string Name;
        float MassNumber = 0.0f;
        bool IsStable = false;
        float NaturalAbundance = 0.0f;
        std::optional<float> HalfLife;
        std::optional<std::string> RadiationType;
    };

    struct PropertyWithUnit {
        std::optional<float> Value;
        std::string Unit;
    };

    struct TriplePoint {
        PropertyWithUnit Temperature;
        PropertyWithUnit Pressure;
    };

    struct PhysicalPropertiesBlock {
        PropertyWithUnit Density;
        PropertyWithUnit MeltingPoint;
        PropertyWithUnit BoilingPoint;
        TriplePoint TriplePoint;
        PropertyWithUnit HeatCapacity;
        PropertyWithUnit ThermalConductivity;
        PropertyWithUnit EnthalpyOfFusion;
        PropertyWithUnit EnthalpyOfVaporization;
        PropertyWithUnit Entropy;
    };

    struct RangeEndpoint {
        float Value = 0.0f;
        std::string Unit;
    };

    struct Range {
        RangeEndpoint Min;
        RangeEndpoint Max;
    };

    struct StateRange {
        Range Temperature;
        Range Pressure;
        std::string State;
    };

    struct ScalarProperty {
        std::optional<float> Value;
        std::string Unit;
    };

    struct IonizationEnergy {
        ScalarProperty First;
        // Extendable: Second, Third, etc.
    };

    struct OxidationState {
        int State = 0;
        bool Common = false;
    };

    struct Reactivity {
        float Value = 0.0f;
        std::string Scale;
    };

    struct ChemicalProperties {
        ScalarProperty Electronegativity;
        IonizationEnergy IonizationEnergy;
        ScalarProperty ElectronAffinity;
        ScalarProperty StandardReductionPotential;
        Reactivity Reactivity;
        std::vector<OxidationState> OxidationStates;
        std::vector<std::string> Bonding;
        std::vector<std::string> CommonCompounds;
        std::vector<std::pair<std::string, std::string>> HybridizationExamples;
    };

    struct QuantumProperties {
        IntUnit AtomicRadius;
        IntUnit CovalentRadius;
        IntUnit VanDerWaalsRadius;
        std::string MagneticOrdering;
        std::vector<float> SpinStates;
        bool IsParamagnetic = false;
        float NuclearSpin = 0.0f;
    };

    struct EnvironmentalSolubility {
        std::optional<float> Value;
        std::optional<std::string> Unit;
    };

    struct OxidationInAirInfo {
        std::string Tendency;
        bool Spontaneous = false;
        bool RequiresSpark = false; // Optional
    };

    struct EnvironmentalProperties {
        EnvironmentalSolubility SolubilityInWater;
        OxidationInAirInfo OxidationInAir;
        std::string Toxicity;
        std::string BiologicalRole;
        std::vector<std::string> Hazards;
    };


    inline auto s_Logger = Interstellar::Core::Logger("ElementConfig", Interstellar::Core::LogLevel::Info);
    /**
     * @ingroup InterstellarConfig
     * @brief JSON-based configuration class for deserializing chemical element data.
     *
     * This class allows parsing of extended periodic table data from configuration files.
     * It is a bridge between JSON representation and runtime-ready ElementData.
     *
     * @since 1.0
     */
    class ElementConfig : public JsonConfigBase {
    public:
        // === Identification ===
        std::string Symbol;             ///< Atomic symbol (e.g. "H")
        std::string Name;               ///< Full element name
        int AtomicNumber = 0;           ///< Proton count
        float AtomicMass = 0.f;

        // === Physical Properties ===
        int Group = 0;
        int Period = 0;
        std::string Block;
        std::string StandardState;

        glm::vec3 Color = { 1.f, 1.f, 1.f };
        ConfigCosmicAbundance CosmicAbundance;
        ConfigElectronConfiguration ElectronConfiguration;
        std::vector<int> ElectronShells;
        int ValenceElectrons = 0;
        std::vector<ConfigIsotope> Isotopes;
        PhysicalPropertiesBlock PhysicalProperties;
        std::vector<StateRange> StateRanges;
        ChemicalProperties Chemical;
        QuantumProperties Quantum;
        EnvironmentalProperties EnvironmentalProp;


        /**
         * @brief Constructor that registers fields for JSON serialization/deserialization.
         */
        ElementConfig() {
            using json = nlohmann::json;

            // Identification
            registerField("Symbol", [this]() { return Symbol; }, [this](const json& v) { Symbol = v.get<std::string>(); });
            registerField("Name", [this]() { return Name; }, [this](const json& v) { Name = v.get<std::string>(); });
            registerField("AtomicNumber", [this]() { return AtomicNumber; }, [this](const json& v) { AtomicNumber = v.get<int>(); });

            // Physical
            registerField("AtomicMass", [this]() { return AtomicMass; }, [this](const json& v) { AtomicMass = v.get<float>(); });
            registerField("Group", [this]() { return Group; }, [this](const json& v) { Group = v.is_number_integer() ? v.get<int>() : 0; });
            registerField("Period", [this]() { return Period; }, [this](const json& v) { Period = v.is_number_integer() ? v.get<int>() : 0; });
            registerField("Block", [this]() { return Block; }, [this](const json& v) { Block = v.get<std::string>(); });
            registerField("StandardState", [this]() { return StandardState; }, [this](const json& v) { StandardState = v.get<std::string>(); });
            registerField("Color", [this]() { return std::vector<float>{ Color.r, Color.g, Color.b }; }, [this](const json& v) {
                    auto vec = v.get<std::vector<float>>();
                    if (vec.size() == 3)
                        Color = glm::vec3(vec[0], vec[1], vec[2]);
                });
            registerField("CosmicAbundance", [this]() { return nlohmann::json{ { "Value", CosmicAbundance.Value }, { "Unit", CosmicAbundance.Unit } }; }, [this](const json& v) {
                    if (v.contains("Value") and v["Value"].is_number())
                        CosmicAbundance.Value = v["Value"].get<float>();

                    if (v.contains("Unit") && v["Unit"].is_string())
                        CosmicAbundance.Unit = v["Unit"].get<std::string>();
                });
            registerField("ElectronConfiguration", [this]() { return ElectronConfiguration.Display; }, [this](const json& v) {
                    if (v.is_string()) {
                        ElectronConfiguration.Display = v.get<std::string>();
                        auto temp = parseElectronConfiguration(ElectronConfiguration.Display);
                        ElectronConfiguration.NobleGas = Interstellar::Utils::extractBracketContent(temp.NobleGas);
                        ElectronConfiguration.Orbitals = temp.Orbitals;

                    }
                });
            registerField("ElectronShells", [this]() { return ElectronShells; }, [this](const json& v) {
                    if (v.is_array())
                        ElectronShells = v.get<std::vector<int>>();
                });
            registerField("ValenceElectrons", [this]() { return ValenceElectrons; }, [this](const json& v) { ValenceElectrons = v.is_number_integer() ? v.get<int>() : 0;  });
            registerField("Isotopes",
                [this]() {
                    json array = json::array();
                    for (const auto& iso : Isotopes) {
                        json j;
                        j["Name"] = iso.Name;
                        j["MassNumber"] = iso.MassNumber;
                        j["IsStable"] = iso.IsStable;
                        j["NaturalAbundance"] = iso.NaturalAbundance;
                        j["HalfLife"] = iso.HalfLife.has_value() ? json(iso.HalfLife.value()) : json(nullptr);
                        j["RadiationType"] = iso.RadiationType.has_value() ? json(iso.RadiationType.value()) : json(nullptr);
                        array.push_back(j);
                    }
                    return array;
                },
                [this](const json& v) {
                    if (!v.is_array()) return;
                    Isotopes.clear();
                    for (const auto& item : v) {
                        ConfigIsotope iso;
                        if (item.contains("Name")) iso.Name = item["Name"].get<std::string>();
                        if (item.contains("MassNumber")) iso.MassNumber = item["MassNumber"].get<float>();
                        if (item.contains("IsStable")) iso.IsStable = item["IsStable"].get<bool>();
                        if (item.contains("NaturalAbundance")) iso.NaturalAbundance = item["NaturalAbundance"].get<float>();
                        iso.HalfLife = (item.contains("HalfLife") && item["HalfLife"].is_number_float())
                            ? std::optional<float>(item["HalfLife"].get<float>())
                            : std::nullopt;

                        iso.RadiationType = (item.contains("RadiationType") && item["RadiationType"].is_string())
                            ? std::optional<std::string>(item["RadiationType"].get<std::string>())
                            : std::nullopt;

                        Isotopes.push_back(iso);
                    }
                });

            registerField("PhysicalProperties",
                [this]() {
                    auto propToJson = [](const PropertyWithUnit& p) {
                        return json{
                            { "Value", p.Value.has_value() ? json(p.Value.value()) : json(nullptr) },
                            { "Unit", p.Unit }
                        };
                        };
                    return json{
                        { "Density", propToJson(PhysicalProperties.Density) },
                        { "MeltingPoint", propToJson(PhysicalProperties.MeltingPoint) },
                        { "BoilingPoint", propToJson(PhysicalProperties.BoilingPoint) },
                        { "TriplePoint", {
                            { "Temperature", propToJson(PhysicalProperties.TriplePoint.Temperature) },
                            { "Pressure", propToJson(PhysicalProperties.TriplePoint.Pressure) }
                        }},
                        { "HeatCapacity", propToJson(PhysicalProperties.HeatCapacity) },
                        { "ThermalConductivity", propToJson(PhysicalProperties.ThermalConductivity) },
                        { "EnthalpyOfFusion", propToJson(PhysicalProperties.EnthalpyOfFusion) },
                        { "EnthalpyOfVaporization", propToJson(PhysicalProperties.EnthalpyOfVaporization) },
                        { "Entropy", propToJson(PhysicalProperties.Entropy) }
                    };
                },
                [this](const json& v) {
                    auto readProp = [](const json& j, PropertyWithUnit& p) {
                        if (j.contains("Value") && !j["Value"].is_null())
                            p.Value = j["Value"].get<float>();
                        else
                            p.Value.reset();
                        if (j.contains("Unit") && j["Unit"].is_string())
                            p.Unit = j["Unit"].get<std::string>();
                        };

                    if (v.contains("Density")) readProp(v["Density"], PhysicalProperties.Density);
                    if (v.contains("MeltingPoint")) readProp(v["MeltingPoint"], PhysicalProperties.MeltingPoint);
                    if (v.contains("BoilingPoint")) readProp(v["BoilingPoint"], PhysicalProperties.BoilingPoint);
                    if (v.contains("TriplePoint")) {
                        const auto& tp = v["TriplePoint"];
                        if (tp.contains("Temperature")) readProp(tp["Temperature"], PhysicalProperties.TriplePoint.Temperature);
                        if (tp.contains("Pressure")) readProp(tp["Pressure"], PhysicalProperties.TriplePoint.Pressure);
                    }
                    if (v.contains("HeatCapacity")) readProp(v["HeatCapacity"], PhysicalProperties.HeatCapacity);
                    if (v.contains("ThermalConductivity")) readProp(v["ThermalConductivity"], PhysicalProperties.ThermalConductivity);
                    if (v.contains("EnthalpyOfFusion")) readProp(v["EnthalpyOfFusion"], PhysicalProperties.EnthalpyOfFusion);
                    if (v.contains("EnthalpyOfVaporization")) readProp(v["EnthalpyOfVaporization"], PhysicalProperties.EnthalpyOfVaporization);
                    if (v.contains("Entropy")) readProp(v["Entropy"], PhysicalProperties.Entropy);
                });

            registerField("StateRanges",
                [this]() {
                    json out = json::array();
                    for (const auto& sr : StateRanges) {
                        out.push_back({
                            { "TemperatureRange", {
                                { { "Value", sr.Temperature.Min.Value }, { "Unit", sr.Temperature.Min.Unit } },
                                { { "Value", sr.Temperature.Max.Value }, { "Unit", sr.Temperature.Max.Unit } }
                            }},
                            { "PressureRange", {
                                { { "Value", sr.Pressure.Min.Value }, { "Unit", sr.Pressure.Min.Unit } },
                                { { "Value", sr.Pressure.Max.Value }, { "Unit", sr.Pressure.Max.Unit } }
                            }},
                            { "State", sr.State }
                            });
                    }
                    return out;
                },
                [this](const json& v) {
                    StateRanges.clear();
                    if (!v.is_array()) return;

                    for (const auto& item : v) {
                        StateRange sr;

                        auto parseRange = [](const json& arr, Range& range) {
                            if (arr.is_array() && arr.size() == 2) {
                                if (arr[0].contains("Value") && arr[0]["Value"].is_number())
                                    range.Min.Value = arr[0]["Value"].get<float>();
                                if (arr[0].contains("Unit"))
                                    range.Min.Unit = arr[0]["Unit"].get<std::string>();

                                if (arr[1].contains("Value") && arr[1]["Value"].is_number())
                                    range.Max.Value = arr[1]["Value"].get<float>();
                                if (arr[1].contains("Unit"))
                                    range.Max.Unit = arr[1]["Unit"].get<std::string>();
                            }
                            };

                        if (item.contains("TemperatureRange"))
                            parseRange(item["TemperatureRange"], sr.Temperature);
                        if (item.contains("PressureRange"))
                            parseRange(item["PressureRange"], sr.Pressure);
                        if (item.contains("State") && item["State"].is_string())
                            sr.State = item["State"].get<std::string>();

                        StateRanges.push_back(sr);
                    }
                });
            registerField("ChemicalProperties",
                [this]() {
                    json out;

                    // Electronegativity
                    if (Chemical.Electronegativity.Value.has_value()) {
                        out["Electronegativity"] = {
                            { "Value", Chemical.Electronegativity.Value.value() },
                            { "Unit", Chemical.Electronegativity.Unit }
                        };
                    }

                    // IonizationEnergy
                    out["IonizationEnergy"]["First"] = {
                        { "Value", Chemical.IonizationEnergy.First.Value.value_or(0.0f) },
                        { "Unit", Chemical.IonizationEnergy.First.Unit }
                    };

                    // ElectronAffinity
                    if (Chemical.ElectronAffinity.Value.has_value()) {
                        out["ElectronAffinity"] = {
                            { "Value", Chemical.ElectronAffinity.Value.value() },
                            { "Unit", Chemical.ElectronAffinity.Unit }
                        };
                    }

                    // StandardReductionPotential
                    if (Chemical.StandardReductionPotential.Value.has_value()) {
                        out["StandardReductionPotential"] = {
                            { "Value", Chemical.StandardReductionPotential.Value.value() },
                            { "Unit", Chemical.StandardReductionPotential.Unit }
                        };
                    }

                    // Reactivity
                    out["Reactivity"] = {
                        { "Value", Chemical.Reactivity.Value },
                        { "Scale", Chemical.Reactivity.Scale }
                    };

                    // OxidationStates
                    for (const auto& ox : Chemical.OxidationStates)
                        out["OxidationStates"].push_back({ { "State", ox.State }, { "Common", ox.Common } });

                    // Bonding
                    out["Bonding"] = Chemical.Bonding;

                    // Compounds
                    out["CommonCompounds"] = Chemical.CommonCompounds;

                    // Hybridizations
                    for (const auto& pair : Chemical.HybridizationExamples)
                        out["HybridizationExamples"].push_back({ { "Molecule", pair.first }, { "Hybridization", pair.second } });

                    return out;
                },
                [this](const json& v) {
                    if (v.contains("Electronegativity")) {
                        const auto& el = v["Electronegativity"];
                        if (el.contains("Value") && !el["Value"].is_null())
                            Chemical.Electronegativity.Value = el["Value"].get<float>();
                        if (el.contains("Unit"))
                            Chemical.Electronegativity.Unit = el["Unit"].get<std::string>();
                    }

                    if (v.contains("IonizationEnergy") && v["IonizationEnergy"].contains("First")) {
                        const auto& ie = v["IonizationEnergy"]["First"];
                        if (ie.contains("Value") && !ie["Value"].is_null())
                            Chemical.IonizationEnergy.First.Value = ie["Value"].get<float>();
                        if (ie.contains("Unit"))
                            Chemical.IonizationEnergy.First.Unit = ie["Unit"].get<std::string>();
                    }

                    if (v.contains("ElectronAffinity")) {
                        const auto& ea = v["ElectronAffinity"];
                        if (ea.contains("Value") && !ea["Value"].is_null())
                            Chemical.ElectronAffinity.Value = ea["Value"].get<float>();
                        if (ea.contains("Unit"))
                            Chemical.ElectronAffinity.Unit = ea["Unit"].get<std::string>();
                    }

                    if (v.contains("StandardReductionPotential")) {
                        const auto& rp = v["StandardReductionPotential"];
                        if (rp.contains("Value") && !rp["Value"].is_null())
                            Chemical.StandardReductionPotential.Value = rp["Value"].get<float>();
                        if (rp.contains("Unit"))
                            Chemical.StandardReductionPotential.Unit = rp["Unit"].get<std::string>();
                    }

                    if (v.contains("Reactivity")) {
                        const auto& r = v["Reactivity"];
                        if (r.contains("Value"))
                            Chemical.Reactivity.Value = r["Value"].get<float>();
                        if (r.contains("Scale"))
                            Chemical.Reactivity.Scale = r["Scale"].get<std::string>();
                    }

                    if (v.contains("OxidationStates") && v["OxidationStates"].is_array()) {
                        for (const auto& os : v["OxidationStates"]) {
                            OxidationState o;
                            if (os.contains("State"))
                                o.State = os["State"].get<int>();
                            if (os.contains("Common"))
                                o.Common = os["Common"].get<bool>();
                            Chemical.OxidationStates.push_back(o);
                        }
                    }

                    if (v.contains("Bonding") && v["Bonding"].is_array())
                        Chemical.Bonding = v["Bonding"].get<std::vector<std::string>>();

                    if (v.contains("CommonCompounds") && v["CommonCompounds"].is_array())
                        Chemical.CommonCompounds = v["CommonCompounds"].get<std::vector<std::string>>();

                    if (v.contains("HybridizationExamples") && v["HybridizationExamples"].is_array()) {
                        for (const auto& h : v["HybridizationExamples"]) {
                            if (h.contains("Molecule") && h.contains("Hybridization")) {
                                Chemical.HybridizationExamples.emplace_back(
                                    h["Molecule"].get<std::string>(),
                                    h["Hybridization"].get<std::string>()
                                );
                            }
                        }
                    }
                });

                
            registerField("QuantumProperties",
                [this]() {
                    json out;

                    //if (Quantum.AtomicRadius.Value.has_value())
                        //out["AtomicRadius"] = {
                            //{ "Value", Quantum.AtomicRadius.Value.value() },
                            //{ "Unit", Quantum.AtomicRadius.Unit.value_or("pm")}
                    //};
                    //if (Quantum.CovalentRadius.Value.has_value()) {
                        //out["CovalentRadius"] = {
                            //{ "Value", Quantum.CovalentRadius.Value.value() },
                            //{ "Unit", Quantum.CovalentRadius.Unit.value_or("pm") }
                        //};
                    //}
                    //if (Quantum.VanDerWaalsRadius.Value.has_value()) {
                        //out["VanDerWaalsRadius"] = {
                            //{ "Value", Quantum.VanDerWaalsRadius.Value.value() },
                            //{ "Unit", Quantum.VanDerWaalsRadius.Unit.value_or("pm") }
                        //};
                    //}

                    //out["MagneticOrdering"] = Quantum.MagneticOrdering;
                    //out["SpinStates"] = Quantum.SpinStates;
                    //out["IsParamagnetic"] = Quantum.IsParamagnetic;
                    //out["NuclearSpin"] = Quantum.NuclearSpin;

                    return out;
                },
                [this](const json& v) {
                    auto tryGetRadius = [](const json& field) -> std::pair<std::optional<int>, std::optional<std::string>> {
                        std::optional<int> val;
                        std::optional<std::string> unit;

                        if (field.contains("Value") && field["Value"].is_number())
                            val = field["Value"].get<int>();

                        if (field.contains("Unit") && field["Unit"].is_string())
                            unit = field["Unit"].get<std::string>();

                        return { val, unit };
                        };

                    if (v.contains("AtomicRadius")) {
                        auto [val, unit] = tryGetRadius(v["AtomicRadius"]);
                        Quantum.AtomicRadius.Value = val;
                        Quantum.AtomicRadius.Unit = unit;
                    }

                    if (v.contains("CovalentRadius")) {
                        auto [val, unit] = tryGetRadius(v["CovalentRadius"]);
                        Quantum.CovalentRadius.Value = val;
                        Quantum.CovalentRadius.Unit = unit;
                    }

                    if (v.contains("VanDerWaalsRadius")) {
                        auto [val, unit] = tryGetRadius(v["VanDerWaalsRadius"]);
                        Quantum.VanDerWaalsRadius.Value = val;
                        Quantum.VanDerWaalsRadius.Unit = unit;
                    }

                    if (v.contains("MagneticOrdering"))
                        Quantum.MagneticOrdering = v["MagneticOrdering"].get<std::string>();

                    if (v.contains("SpinStates") && v["SpinStates"].is_array())
                        Quantum.SpinStates = v["SpinStates"].get<std::vector<float>>();

                    if (v.contains("IsParamagnetic") && v["IsParamagnetic"].is_boolean())
                        Quantum.IsParamagnetic = v["IsParamagnetic"].get<bool>();
                    else
                        Quantum.IsParamagnetic = false;

                    if (v.contains("NuclearSpin") && v["NuclearSpin"].is_number())
                        Quantum.NuclearSpin = v["NuclearSpin"].get<float>();
                }
            );

            registerField("EnvironmentalBehavior",
                [this]() {
                    json out;

                    // Solubility
                    //if (EnvironmentalProp.SolubilityInWater.Value.has_value()) {
                        //out["Solubility"]["InWater"]["Value"] = EnvironmentalProp.SolubilityInWater.Value.value();
                    //}
                    //else {
                        //out["Solubility"]["InWater"]["Value"] = nullptr;
                    //}
                    //out["Solubility"]["InWater"]["Unit"] = EnvironmentalProp.SolubilityInWater.Unit.value_or("g/L");

                    // OxidationInAir
                    //out["OxidationInAir"] = {
                        //{ "Tendency", EnvironmentalProp.OxidationInAir.Tendency },
                        //{ "Spontaneous", EnvironmentalProp.OxidationInAir.Spontaneous }
                    //};

                    // Optional: RequiresSpark
                    //out["OxidationInAir"]["RequiresSpark"] = EnvironmentalProp.OxidationInAir.RequiresSpark;

                    // Other strings
                    //out["Toxicity"] = EnvironmentalProp.Toxicity;
                    //out["BiologicalRole"] = EnvironmentalProp.BiologicalRole;
                    //out["Hazards"] = EnvironmentalProp.Hazards;

                    return out;
                },
                [this](const json& v) {
                    if (v.contains("Solubility")) {
                        const auto& sol = v["Solubility"];
                        if (sol.contains("InWater")) {
                            const auto& water = sol["InWater"];
                            if (water.contains("Value") && water["Value"].is_number_float())
                                EnvironmentalProp.SolubilityInWater.Value = water["Value"].get<float>();
                            else
                                EnvironmentalProp.SolubilityInWater.Value = std::nullopt;

                            if (water.contains("Unit"))
                                EnvironmentalProp.SolubilityInWater.Unit = water["Unit"].get<std::string>();
                        }
                    }

                    if (v.contains("OxidationInAir")) {
                        const auto& oxy = v["OxidationInAir"];
                        if (oxy.contains("Tendency"))
                            EnvironmentalProp.OxidationInAir.Tendency = oxy["Tendency"].get<std::string>();
                        if (oxy.contains("Spontaneous"))
                            EnvironmentalProp.OxidationInAir.Spontaneous = oxy["Spontaneous"].get<bool>();
                        if (oxy.contains("RequiresSpark"))
                            EnvironmentalProp.OxidationInAir.RequiresSpark = oxy["RequiresSpark"].get<bool>();
                    }

                    if (v.contains("Toxicity"))
                        EnvironmentalProp.Toxicity = v["Toxicity"].get<std::string>();

                    if (v.contains("BiologicalRole") && !v["BiologicalRole"].is_null())
                        EnvironmentalProp.BiologicalRole = v["BiologicalRole"].get<std::string>();

                    if (v.contains("Hazards") && v["Hazards"].is_array())
                        EnvironmentalProp.Hazards = v["Hazards"].get<std::vector<std::string>>();
                }
            );
        }

        /**
         * @brief Converts this configuration to a runtime-safe ElementData structure.
         *
         * @return Fully constructed ElementData object.
         */
        [[nodiscard]] Interstellar::Data::ElementData toData() const {
            Interstellar::Data::ElementData e;
            e.symbol = Symbol;
            e.name = Name;
            e.atomicNumber = AtomicNumber;
            e.atomicMass = AtomicMass;
            e.group = Group;
            e.period = Period;
            e.block = Block;
            e.standardState = StandardState;
            e.colour = Color;
            e.cosmicAbundance.unit = CosmicAbundance.Unit;
            e.cosmicAbundance.value = CosmicAbundance.Value;
            e.electronConfiguration.display = ElectronConfiguration.Display;
            e.electronConfiguration.nobleGas = ElectronConfiguration.NobleGas;
            for (const auto& orb : ElectronConfiguration.Orbitals) {
                Interstellar::Data::ElementData::ElectronConfiguration::Orbital destOrb;
                destOrb.orbital = orb.Orbital;
                destOrb.electrons = orb.Electrons;
                e.electronConfiguration.orbitals.push_back(destOrb);
            };
            e.electronShells = ElectronShells;
            e.valenceElectrons = ValenceElectrons;
            for (const auto& iso : Isotopes) {
                Interstellar::Data::ElementData::Isotope destIso;
                destIso.name = iso.Name;
                destIso.massNumber = iso.MassNumber;
                destIso.isStable = iso.IsStable;
                destIso.naturalAbundance = iso.NaturalAbundance;
                destIso.halfLife = iso.HalfLife;
                destIso.radiationType = iso.RadiationType;

                e.isotopes.push_back(destIso);
            };

            e.physicalProperties.density.value = PhysicalProperties.Density.Value;
            e.physicalProperties.density.unit = PhysicalProperties.Density.Unit;
            e.physicalProperties.meltingPoint.value = PhysicalProperties.MeltingPoint.Value;
            e.physicalProperties.meltingPoint.unit = PhysicalProperties.MeltingPoint.Unit;
            e.physicalProperties.boilingPoint.value = PhysicalProperties.BoilingPoint.Value;
            e.physicalProperties.boilingPoint.unit = PhysicalProperties.BoilingPoint.Unit;
            e.physicalProperties.triplePoint.temperature.value = PhysicalProperties.TriplePoint.Temperature.Value;
            e.physicalProperties.triplePoint.temperature.unit = PhysicalProperties.TriplePoint.Temperature.Unit;
            e.physicalProperties.heatCapacity.value = PhysicalProperties.HeatCapacity.Value;
            e.physicalProperties.heatCapacity.unit = PhysicalProperties.HeatCapacity.Unit;
            e.physicalProperties.thermalConductivity.value = PhysicalProperties.ThermalConductivity.Value;
            e.physicalProperties.thermalConductivity.unit = PhysicalProperties.ThermalConductivity.Unit;
            e.physicalProperties.enthalpyOfFusion.value = PhysicalProperties.EnthalpyOfFusion.Value;
            e.physicalProperties.enthalpyOfFusion.unit = PhysicalProperties.EnthalpyOfFusion.Unit;
            e.physicalProperties.enthalpyOfVaporization.value = PhysicalProperties.EnthalpyOfVaporization.Value;
            e.physicalProperties.enthalpyOfVaporization.unit = PhysicalProperties.EnthalpyOfVaporization.Unit;
            e.physicalProperties.entropy.value = PhysicalProperties.Entropy.Value;
            e.physicalProperties.entropy.unit = PhysicalProperties.Entropy.Unit;

            for (const auto& state: StateRanges) {
                Interstellar::Data::ElementData::StateRange destState;
                destState.temperature.min.value = state.Temperature.Min.Value;
                destState.temperature.min.unit = state.Temperature.Min.Unit;
                destState.temperature.max.value = state.Temperature.Max.Value;
                destState.temperature.max.unit = state.Temperature.Max.Unit;
                destState.pressure.min.value = state.Pressure.Min.Value;
                destState.pressure.min.unit = state.Pressure.Min.Unit;
                destState.pressure.max.value = state.Pressure.Max.Value;
                destState.pressure.max.unit = state.Pressure.Max.Unit;
                destState.state = state.State;

                e.stateRanges.push_back(destState);
            };

            e.chemical.electronegativity.value = Chemical.Electronegativity.Value;
            e.chemical.electronegativity.unit = Chemical.Electronegativity.Unit;
            e.chemical.ionizationEnergy.first.value = Chemical.IonizationEnergy.First.Value;
            e.chemical.ionizationEnergy.first.unit = Chemical.IonizationEnergy.First.Unit;
            e.chemical.electronAffinity.value = Chemical.ElectronAffinity.Value;
            e.chemical.electronAffinity.unit = Chemical.ElectronAffinity.Unit;
            for (const auto& oxy : Chemical.OxidationStates) {
                Interstellar::Data::ElementData::OxidationState destState;
                destState.state = oxy.State;
                destState.common = oxy.Common;
                e.chemical.oxidationStates.push_back(destState);
            }
            e.chemical.standardReductionPotential.value = Chemical.StandardReductionPotential.Value;
            e.chemical.standardReductionPotential.unit = Chemical.StandardReductionPotential.Unit;
            e.chemical.reactivity.value = Chemical.Reactivity.Value;
            e.chemical.reactivity.scale = Chemical.Reactivity.Scale;
            for (const auto& bond : Chemical.Bonding) {
                e.chemical.bonding.push_back(bond);
            }
            for (const auto& comp : Chemical.CommonCompounds) {
                e.chemical.commonCompounds.push_back(comp);
            }
            for (const auto& hyb : Chemical.HybridizationExamples) {
                e.chemical.hybridizationExamples.push_back(hyb);
            }

            e.quantum.atomicRadius.value = Quantum.AtomicRadius.Value;
            e.quantum.atomicRadius.unit = Quantum.AtomicRadius.Unit;
            e.quantum.covalentRadius.value = Quantum.CovalentRadius.Value;
            e.quantum.covalentRadius.unit = Quantum.CovalentRadius.Unit;
            e.quantum.vanDerWaalsRadius.value = Quantum.VanDerWaalsRadius.Value;
            e.quantum.vanDerWaalsRadius.unit = Quantum.VanDerWaalsRadius.Unit;
            e.quantum.magneticOrdering = Quantum.MagneticOrdering;
            for( const auto& spin : Quantum.SpinStates) {
                e.quantum.spinStates.push_back(spin);
            }
            e.quantum.isParamagnetic = Quantum.IsParamagnetic;
            e.quantum.nuclearSpin = Quantum.NuclearSpin;

            e.environmentalProp.solubilityInWater.value = EnvironmentalProp.SolubilityInWater.Value;
            e.environmentalProp.solubilityInWater.unit = EnvironmentalProp.SolubilityInWater.Unit;
            e.environmentalProp.oxidationInAir.tendency = EnvironmentalProp.OxidationInAir.Tendency;
            e.environmentalProp.oxidationInAir.spontaneous = EnvironmentalProp.OxidationInAir.Spontaneous;
            e.environmentalProp.oxidationInAir.requiresSpark = EnvironmentalProp.OxidationInAir.RequiresSpark;
            e.environmentalProp.toxicity = EnvironmentalProp.Toxicity;
            e.environmentalProp.biologicalRole = EnvironmentalProp.BiologicalRole;
            e.environmentalProp.hazards = EnvironmentalProp.Hazards;

            return e;
        };

        ConfigElectronConfiguration parseElectronConfiguration(const std::string& input) {
            ConfigElectronConfiguration config;
            config.NobleGas = "";  // Default if not found

            std::istringstream iss(input);
            std::string token;

            // Regex for orbital like "4f5", "6s2", etc.
            std::regex orbitalPattern(R"((\d+[spdf])(\d+))");

            while (iss >> token) {
                // Check if token is a noble gas (e.g., [Xe])
                if (token.front() == '[' && token.back() == ']') {
                    config.NobleGas = token;
                }
                // Else, try to parse as orbital
                else {
                    std::smatch match;
                    if (std::regex_match(token, match, orbitalPattern)) {
                        ConfigOrbital orb;
                        orb.Orbital = match[1].str();
                        orb.Electrons = std::stoi(match[2].str());
                        config.Orbitals.push_back(orb);
                    }
                    else {
                        std::cerr << "Warning: Unrecognized token: " << token << "\n";
                    }
                }
            }

            return config;
        }

        /// @brief The namespace used when serializing this config
        std::string getNamespace() const override { return "data"; }

        /// @brief The filename from which to load this configuration
        std::string getFilename() const override { return "elements_all.json"; }

    };

} // namespace Interstellar::Config
