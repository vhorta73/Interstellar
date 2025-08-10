#include <sstream>
#include <regex>

#include "Interstellar/Config/JsonImpl/ElementConfig.hpp"
#include "Interstellar/Utils/Json/JsonHelpers.hpp"

using json = nlohmann::json;

namespace Interstellar::Config::JsonImpl {

    using namespace Interstellar::Utils::Json;
    using namespace Interstellar::Utils;
    using namespace Interstellar::Data;

    /**
     * @brief Logger instance for ElementConfig operations.
     */
    inline auto s_Logger = Interstellar::Core::Logger(Interstellar::Core::LOG_CONFIG);

    /**
     * @brief Constructs the ElementConfig object and registers all JSON fields.
     */
    ElementConfig::ElementConfig() {
        registerAllFields();
    }

    /**
    * @brief Populates all registered JSON fields for serialization/deserialization.
    *
    * This method is tightly coupled to the structure of ElementConfig and must be kept
    * synchronized with both the JSON schema and the ElementData runtime model.
    */
    void ElementConfig::registerAllFields() {
        using json = nlohmann::json;

        // Identification
        registerField("Symbol",
            [this]() { return Symbol; },
            [this](const json& v) { Symbol = v.get<std::string>(); }
        );

        registerField("Name",
            [this]() { return Name; },
            [this](const json& v) { Name = v.get<std::string>(); }
        );

        registerField("AtomicNumber",
            [this]() { return AtomicNumber; },
            [this](const json& v) { AtomicNumber = get_json_to_int(v); }
        );

        // Physical
        registerField("AtomicMass",
            [this]() { return AtomicMass; },
            [this](const json& v) { AtomicMass = get_json_to_float(v); }
        );

        registerField("Group",
            [this]() -> nlohmann::json {
                if (Group.has_value()) {
                    return *Group; // numeric
                }
                if (Series.has_value()) {
                    return element_series_to_string(*Series); // string representation
                }
                return nullptr; // null when neither
            },
            [this](const json& v) { 
                Group.reset();
                Series.reset();

                if (v.is_number_integer()) {
                    Group = v.get<int>();
                }
                else if (v.is_number_float()) {
                    Group = static_cast<int>(v.get<double>());
                }
                else if (v.is_string()) {
                    Series = string_to_element_series(v.get<std::string>());
                }
                // else: null or invalid -> both remain empty
            }
        );
        
        registerField("Period",
            [this]() { return Period; },
            [this](const json& v) { Period = get_json_to_int(v); }
        );

        registerField("Block",
            [this]() { return Block; },
            [this](const json& v) { Block = v.get<std::string>(); }
        );
        
        registerField("StandardState",
            [this]() { return StandardState; },
            [this](const json& v) { StandardState = v.get<std::string>(); }
        );
        
        registerField("Color",
            [this]() { return std::vector<float>{ Colour.r, Colour.g, Colour.b, Colour.a }; },
            [this](const json& v) {
                auto vec = v.get<std::vector<float>>();
                if (vec.size() == 4)
                    Colour = glm::vec4(vec[0], vec[1], vec[2], vec[3]);

                if (v.is_array() && v.size() >= 3) {
                    Colour.r = v[0].get<float>();
                    Colour.g = v[1].get<float>();
                    Colour.b = v[2].get<float>();
                    Colour.a = v.size() >= 4 ? v[3].get<float>() : 1.0f;

                    s_Logger.LogCritical("Loaded color for {}: {}, {}, {}, {}",
                        Symbol, Colour.r, Colour.g, Colour.b, Colour.a);
                }
            }
        );
        registerField("CosmicAbundance",
            [this]() {
                return nlohmann::json{
                    { "Value", CosmicAbundance.value.has_value() ? nlohmann::json(*CosmicAbundance.value) : nlohmann::json(nullptr) },
                    { "Unit",  CosmicAbundance.unit }
                };
            },
            [this](const json& v) {
                // Handle Value (may be missing or null)
                if (v.contains("Value") && !v["Value"].is_null()) {
                    if (v["Value"].is_number()) {
                        CosmicAbundance.value = v["Value"].get<float>();
                    }
                    else {
                        // Invalid type -> reset
                        CosmicAbundance.value.reset();
                    }
                }
                else {
                    CosmicAbundance.value.reset();
                }

                // Handle Unit (optional)
                if (v.contains("Unit") && v["Unit"].is_string()) {
                    CosmicAbundance.unit = v["Unit"].get<std::string>();
                }
                else {
                    CosmicAbundance.unit.clear();
                }
            }
        );

        // Assuming member variable is named `ElectronConfiguration ElectronConfiguration;`
        registerField(
            "ElectronConfiguration",
            // Getter: keep JSON as the canonical string form
            [this]() { return nlohmann::json(ElectronConfiguration.display); },

            // Setter: accept string, parse into structured fields
            [this](const nlohmann::json& v) {
                // Reset first to avoid stale data if parsing fails
                ElectronConfiguration.nobleGas.clear();
                ElectronConfiguration.orbitals.clear();
                ElectronConfiguration.display.clear();

                if (!v.is_string()) {
                    // not a string -> leave cleared; optionally log a warning
                    // s_Logger.LogWarn("ElectronConfiguration must be a string");
                    return;
                }

                ElectronConfiguration.display = v.get<std::string>();

                // Parse the display string into nobleGas + orbitals
                auto parsed = parseElectronConfiguration(ElectronConfiguration.display);

                // If your parser returns the same struct type, great:
                // (otherwise adapt field names accordingly)
                ElectronConfiguration.nobleGas = Interstellar::Utils::Json::extractBracketContent(parsed.nobleGas);
                ElectronConfiguration.orbitals = std::move(parsed.orbitals);
            }
        );
        
        registerField("ElectronShells",
            [this]() { return ElectronShells; },
            [this](const json& v) { if (v.is_array()) ElectronShells = v.get<std::vector<int>>(); }
        );

        registerField("ValenceElectrons",
            [this]() { return ValenceElectrons; },
            [this](const json& v) { ValenceElectrons = v.is_number_integer() ? v.get<int>() : 0; }
        );
        
        registerField("Isotopes",
            // ---- getter: serialize to JSON ----
            [this]() {
                json array = json::array();
                for (const auto& iso : Isotopes) {
                    json j;
                    j["Name"] = iso.name;
                    j["MassNumber"] = iso.massNumber;
                    j["IsStable"] = iso.isStable;
                    j["NaturalAbundance"] = iso.naturalAbundance;

                    if (iso.halfLife.has_value()) {
                        j["HalfLife"] = {
                            { "Value", iso.halfLife->value },
                            { "Unit",  Interstellar::Utils::Time::time_unit_to_string(iso.halfLife->unit) }
                        };
                    }
                    else {
                        j["HalfLife"] = nullptr;
                    }

                    j["RadiationType"] = iso.radiationType
                        ? json(*iso.radiationType)
                        : json(nullptr);

                    array.push_back(std::move(j));
                }
                return array;
            },

            // ---- setter: parse from JSON ----
            [this](const json& v) {
                if (!v.is_array()) return;

                Isotopes.clear();
                Isotopes.reserve(v.size());

                for (const auto& item : v) {
                    ElementIsotope iso;

                    if (item.contains("Name") && item["Name"].is_string())
                        iso.name = item["Name"].get<std::string>();

                    if (item.contains("MassNumber") && item["MassNumber"].is_number())
                        iso.massNumber = item["MassNumber"].get<float>();

                    if (item.contains("IsStable") && item["IsStable"].is_boolean())
                        iso.isStable = item["IsStable"].get<bool>();

                    if (item.contains("NaturalAbundance")) {
                        const auto& na = item["NaturalAbundance"];
                        if (na.is_number())        iso.naturalAbundance = na.get<float>();
                        else if (na.is_null())     iso.naturalAbundance = 0.0f; // your convention
                    }

                    // HalfLife: null | object {Value, Unit} | number (seconds)
                    if (item.contains("HalfLife")) {
                        const auto& hl = item["HalfLife"];

                        if (hl.is_null()) {
                            iso.halfLife.reset();
                        }
                        else if (hl.is_object()) {
                            double value = 0.0;
                            bool   haveValue = false;
                            std::string unitStr = "seconds";

                            if (hl.contains("Value") && hl["Value"].is_number()) {
                                value = hl["Value"].get<double>();
                                haveValue = true;
                            }
                            if (hl.contains("Unit") && hl["Unit"].is_string()) {
                                unitStr = hl["Unit"].get<std::string>();
                            }

                            if (haveValue) {
                                try {
                                    auto unit = Interstellar::Utils::Time::string_to_time_unit(unitStr);
                                    iso.halfLife = Interstellar::Utils::Time::TimeValueUnit{ value, unit };
                                }
                                catch (const std::invalid_argument&) {
                                    iso.halfLife.reset(); // unknown unit -> ignore
                                }
                            }
                            else {
                                iso.halfLife.reset();
                            }
                        }
                        else if (hl.is_number()) {
                            // Be forgiving: interpret bare number as seconds
                            iso.halfLife = Interstellar::Utils::Time::TimeValueUnit{
                                hl.get<double>(),
                                Interstellar::Utils::Time::TimeUnit::Seconds
                            };
                        }
                        else {
                            // Old/unsupported shapes (e.g., single string) -> ignore
                            iso.halfLife.reset();
                        }
                    }
                    else {
                        iso.halfLife.reset();
                    }

                    if (item.contains("RadiationType")) {
                        const auto& rt = item["RadiationType"];
                        if (rt.is_string()) iso.radiationType = rt.get<std::string>();
                        else                iso.radiationType.reset();
                    }
                    else {
                        iso.radiationType.reset();
                    }

                    Isotopes.push_back(std::move(iso));
                }
            }
        );



            //registerField("PhysicalProperties",
            //    [this]() {
            //        auto propToJson = [](const PropertyWithUnit& p) {
            //            return json{
            //                { "Value", p.Value.has_value() ? json(p.Value.value()) : json(nullptr) },
            //                { "Unit", p.Unit }
            //            };
            //            };
            //        return json{
            //            { "Density", propToJson(PhysicalProperties.Density) },
            //            { "MeltingPoint", propToJson(PhysicalProperties.MeltingPoint) },
            //            { "BoilingPoint", propToJson(PhysicalProperties.BoilingPoint) },
            //            { "TriplePoint", {
            //                { "Temperature", propToJson(PhysicalProperties.TriplePoint.Temperature) },
            //                { "Pressure", propToJson(PhysicalProperties.TriplePoint.Pressure) }
            //            }},
            //            { "HeatCapacity", propToJson(PhysicalProperties.HeatCapacity) },
            //            { "ThermalConductivity", propToJson(PhysicalProperties.ThermalConductivity) },
            //            { "EnthalpyOfFusion", propToJson(PhysicalProperties.EnthalpyOfFusion) },
            //            { "EnthalpyOfVaporization", propToJson(PhysicalProperties.EnthalpyOfVaporization) },
            //            { "Entropy", propToJson(PhysicalProperties.Entropy) }
            //        };
            //    },
            //    [this](const json& v) {
            //        auto readProp = [](const json& j, PropertyWithUnit& p) {
            //            if (j.contains("Value") && !j["Value"].is_null())
            //                p.Value = j["Value"].get<float>();
            //            else
            //                p.Value.reset();
            //            if (j.contains("Unit") && j["Unit"].is_string())
            //                p.Unit = j["Unit"].get<std::string>();
            //            };

                //    if (v.contains("Density")) readProp(v["Density"], PhysicalProperties.Density);
                //    if (v.contains("MeltingPoint")) readProp(v["MeltingPoint"], PhysicalProperties.MeltingPoint);
                //    if (v.contains("BoilingPoint")) readProp(v["BoilingPoint"], PhysicalProperties.BoilingPoint);
                //    if (v.contains("TriplePoint")) {
                //        const auto& tp = v["TriplePoint"];
                //        if (tp.contains("Temperature")) readProp(tp["Temperature"], PhysicalProperties.TriplePoint.Temperature);
                //        if (tp.contains("Pressure")) readProp(tp["Pressure"], PhysicalProperties.TriplePoint.Pressure);
                //    }
                //    if (v.contains("HeatCapacity")) readProp(v["HeatCapacity"], PhysicalProperties.HeatCapacity);
                //    if (v.contains("ThermalConductivity")) readProp(v["ThermalConductivity"], PhysicalProperties.ThermalConductivity);
                //    if (v.contains("EnthalpyOfFusion")) readProp(v["EnthalpyOfFusion"], PhysicalProperties.EnthalpyOfFusion);
                //    if (v.contains("EnthalpyOfVaporization")) readProp(v["EnthalpyOfVaporization"], PhysicalProperties.EnthalpyOfVaporization);
                //    if (v.contains("Entropy")) readProp(v["Entropy"], PhysicalProperties.Entropy);
                //});

            //registerField("StateRanges",
            //    [this]() {
            //        json out = json::array();
            //        for (const auto& sr : StateRanges) {
            //            out.push_back({
            //                { "TemperatureRange", {
            //                    { { "Value", sr.Temperature.Min.Value }, { "Unit", sr.Temperature.Min.Unit } },
            //                    { { "Value", sr.Temperature.Max.Value }, { "Unit", sr.Temperature.Max.Unit } }
            //                }},
            //                { "PressureRange", {
            //                    { { "Value", sr.Pressure.Min.Value }, { "Unit", sr.Pressure.Min.Unit } },
            //                    { { "Value", sr.Pressure.Max.Value }, { "Unit", sr.Pressure.Max.Unit } }
            //                }},
            //                { "State", sr.State }
            //                });
            //        }
            //        return out;
            //    },
            //    [this](const json& v) {
            //        StateRanges.clear();
            //        if (!v.is_array()) return;

                    //for (const auto& item : v) {
                    //    StateRange sr;

                        //auto parseRange = [](const json& arr, Range& range) {
                        //    if (arr.is_array() && arr.size() == 2) {
                        //        if (arr[0].contains("Value") && arr[0]["Value"].is_number())
                        //            range.Min.Value = arr[0]["Value"].get<float>();
                        //        if (arr[0].contains("Unit"))
                        //            range.Min.Unit = arr[0]["Unit"].get<std::string>();

                            //    if (arr[1].contains("Value") && arr[1]["Value"].is_number())
                            //        range.Max.Value = arr[1]["Value"].get<float>();
                            //    if (arr[1].contains("Unit"))
                            //        range.Max.Unit = arr[1]["Unit"].get<std::string>();
                            //}
                            //};

                        //if (item.contains("TemperatureRange"))
                        //    parseRange(item["TemperatureRange"], sr.Temperature);
                        //if (item.contains("PressureRange"))
                        //    parseRange(item["PressureRange"], sr.Pressure);
                        //if (item.contains("State") && item["State"].is_string())
                        //    sr.State = item["State"].get<std::string>();

                        //StateRanges.push_back(sr);
                    //}
                //});
            //registerField("ChemicalProperties",
                //[this]() {
                    //json out;

                    // Electronegativity
                    //if (Chemical.Electronegativity.Value.has_value()) {
                        //out["Electronegativity"] = {
                            //{ "Value", Chemical.Electronegativity.Value.value() },
                            //{ "Unit", Chemical.Electronegativity.Unit }
                        //};
                    //}

                    // IonizationEnergy
                    //out["IonizationEnergy"]["First"] = {
                    //    { "Value", Chemical.IonizationEnergy.First.Value.value_or(0.0f) },
                    //    { "Unit", Chemical.IonizationEnergy.First.Unit }
                    //};

                    // ElectronAffinity
                    //if (Chemical.ElectronAffinity.Value.has_value()) {
                    //    out["ElectronAffinity"] = {
                    //        { "Value", Chemical.ElectronAffinity.Value.value() },
                    //        { "Unit", Chemical.ElectronAffinity.Unit }
                    //    };
                    //}

                    // StandardReductionPotential
                    //if (Chemical.StandardReductionPotential.Value.has_value()) {
                    //    out["StandardReductionPotential"] = {
                    //        { "Value", Chemical.StandardReductionPotential.Value.value() },
                    //        { "Unit", Chemical.StandardReductionPotential.Unit }
                    //    };
                    //}

                    // Reactivity
                    //out["Reactivity"] = {
                    //    { "Value", Chemical.Reactivity.Value },
                    //    { "Scale", Chemical.Reactivity.Scale }
                    //};

                    // OxidationStates
                    //for (const auto& ox : Chemical.OxidationStates)
                    //    out["OxidationStates"].push_back({ { "State", ox.State }, { "Common", ox.Common } });

                    // Bonding
                    //out["Bonding"] = Chemical.Bonding;

                    // Compounds
                    //out["CommonCompounds"] = Chemical.CommonCompounds;

                    // Hybridizations
                    //for (const auto& pair : Chemical.HybridizationExamples)
                        //out["HybridizationExamples"].push_back({ { "Molecule", pair.first }, { "Hybridization", pair.second } });

                    //return out;
                //},
                //[this](const json& v) {
                //    if (v.contains("Electronegativity")) {
                //        const auto& el = v["Electronegativity"];
                //        if (el.contains("Value") && !el["Value"].is_null())
                //            Chemical.Electronegativity.Value = el["Value"].get<float>();
                //        if (el.contains("Unit"))
                //            Chemical.Electronegativity.Unit = el["Unit"].get<std::string>();
                //    }

                    //if (v.contains("IonizationEnergy") && v["IonizationEnergy"].contains("First")) {
                    //    const auto& ie = v["IonizationEnergy"]["First"];
                    //    if (ie.contains("Value") && !ie["Value"].is_null())
                    //        Chemical.IonizationEnergy.First.Value = ie["Value"].get<float>();
                    //    if (ie.contains("Unit"))
                    //        Chemical.IonizationEnergy.First.Unit = ie["Unit"].get<std::string>();
                    //}

                    //if (v.contains("ElectronAffinity")) {
                    //    const auto& ea = v["ElectronAffinity"];
                    //    if (ea.contains("Value") && !ea["Value"].is_null())
                    //        Chemical.ElectronAffinity.Value = ea["Value"].get<float>();
                    //    if (ea.contains("Unit"))
                    //        Chemical.ElectronAffinity.Unit = ea["Unit"].get<std::string>();
                    //}

                    //if (v.contains("StandardReductionPotential")) {
                    //    const auto& rp = v["StandardReductionPotential"];
                    //    if (rp.contains("Value") && !rp["Value"].is_null())
                    //        Chemical.StandardReductionPotential.Value = rp["Value"].get<float>();
                    //    if (rp.contains("Unit"))
                    //        Chemical.StandardReductionPotential.Unit = rp["Unit"].get<std::string>();
                    //}

                    //if (v.contains("Reactivity")) {
                    //    const auto& r = v["Reactivity"];
                    //    if (r.contains("Value"))
                    //        Chemical.Reactivity.Value = r["Value"].get<float>();
                    //    if (r.contains("Scale"))
                    //        Chemical.Reactivity.Scale = r["Scale"].get<std::string>();
                    //}

                    //if (v.contains("OxidationStates") && v["OxidationStates"].is_array()) {
                    //    for (const auto& os : v["OxidationStates"]) {
                    //        OxidationState o;
                    //        if (os.contains("State"))
                    //            o.State = os["State"].get<int>();
                    //        if (os.contains("Common"))
                    //            o.Common = os["Common"].get<bool>();
                    //        Chemical.OxidationStates.push_back(o);
                    //    }
                    //}

                    //if (v.contains("Bonding") && v["Bonding"].is_array())
                    //    Chemical.Bonding = v["Bonding"].get<std::vector<std::string>>();

                    //if (v.contains("CommonCompounds") && v["CommonCompounds"].is_array())
                    //    Chemical.CommonCompounds = v["CommonCompounds"].get<std::vector<std::string>>();

                //    if (v.contains("HybridizationExamples") && v["HybridizationExamples"].is_array()) {
                //        for (const auto& h : v["HybridizationExamples"]) {
                //            if (h.contains("Molecule") && h.contains("Hybridization")) {
                //                Chemical.HybridizationExamples.emplace_back(
                //                    h["Molecule"].get<std::string>(),
                //                    h["Hybridization"].get<std::string>()
                //                );
                //            }
                //        }
                //    }
                //});


            //registerField("QuantumProperties",
                //[this]() {
                    //json out;

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

                    //return out;
                //},
                //[this](const json& v) {
                    //auto tryGetRadius = [](const json& field) -> std::pair<std::optional<int>, std::optional<std::string>> {
                        //std::optional<int> val;
                        //std::optional<std::string> unit;

                        //if (field.contains("Value") && field["Value"].is_number())
                            //val = field["Value"].get<int>();

                        //if (field.contains("Unit") && field["Unit"].is_string())
                            //unit = field["Unit"].get<std::string>();

                        //return { val, unit };
                        //};

                    //if (v.contains("AtomicRadius")) {
                    //    auto [val, unit] = tryGetRadius(v["AtomicRadius"]);
                    //    Quantum.AtomicRadius.Value = val;
                    //    Quantum.AtomicRadius.Unit = unit;
                    //}

                    //if (v.contains("CovalentRadius")) {
                    //    auto [val, unit] = tryGetRadius(v["CovalentRadius"]);
                    //    Quantum.CovalentRadius.Value = val;
                    //    Quantum.CovalentRadius.Unit = unit;
                    //}

                    //if (v.contains("VanDerWaalsRadius")) {
                    //    auto [val, unit] = tryGetRadius(v["VanDerWaalsRadius"]);
                    //    Quantum.VanDerWaalsRadius.Value = val;
                    //    Quantum.VanDerWaalsRadius.Unit = unit;
                    //}

                    //if (v.contains("MagneticOrdering"))
                        //Quantum.MagneticOrdering = v["MagneticOrdering"].get<std::string>();

                    //if (v.contains("SpinStates") && v["SpinStates"].is_array())
                        //Quantum.SpinStates = v["SpinStates"].get<std::vector<float>>();

                    //if (v.contains("IsParamagnetic") && v["IsParamagnetic"].is_boolean())
                        //Quantum.IsParamagnetic = v["IsParamagnetic"].get<bool>();
                    //else
                        //Quantum.IsParamagnetic = false;

                    //if (v.contains("NuclearSpin") && v["NuclearSpin"].is_number())
                        //Quantum.NuclearSpin = v["NuclearSpin"].get<float>();
                //}
            //);

            //registerField("EnvironmentalBehavior",
                //[this]() {
                    //json out;

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

                //    return out;
                //},
                //[this](const json& v) {
                //    if (v.contains("Solubility")) {
                //        const auto& sol = v["Solubility"];
                //        if (sol.contains("InWater")) {
                //            const auto& water = sol["InWater"];
                //            if (water.contains("Value") && water["Value"].is_number_float())
                //                EnvironmentalProp.SolubilityInWater.Value = water["Value"].get<float>();
                //            else
                //                EnvironmentalProp.SolubilityInWater.Value = std::nullopt;

                    //        if (water.contains("Unit"))
                    //            EnvironmentalProp.SolubilityInWater.Unit = water["Unit"].get<std::string>();
                    //    }
                    //}

                    //if (v.contains("OxidationInAir")) {
                    //    const auto& oxy = v["OxidationInAir"];
                    //    if (oxy.contains("Tendency"))
                    //        EnvironmentalProp.OxidationInAir.Tendency = oxy["Tendency"].get<std::string>();
                    //    if (oxy.contains("Spontaneous"))
                    //        EnvironmentalProp.OxidationInAir.Spontaneous = oxy["Spontaneous"].get<bool>();
                    //    if (oxy.contains("RequiresSpark"))
                    //        EnvironmentalProp.OxidationInAir.RequiresSpark = oxy["RequiresSpark"].get<bool>();
                    //}

                    //if (v.contains("Toxicity"))
                    //    EnvironmentalProp.Toxicity = v["Toxicity"].get<std::string>();

                    //if (v.contains("BiologicalRole") && !v["BiologicalRole"].is_null())
                    //    EnvironmentalProp.BiologicalRole = v["BiologicalRole"].get<std::string>();

        //            if (v.contains("Hazards") && v["Hazards"].is_array())
        //                EnvironmentalProp.Hazards = v["Hazards"].get<std::vector<std::string>>();
        //        }
        //    );
        }

    /**
     * @brief Converts this configuration object to the runtime-ready ElementData structure.
     *
     * @return Fully populated ElementData instance.
     */
    Interstellar::Data::ElementData ElementConfig::toData() const {
        Interstellar::Data::ElementData e;

        // === Identification ===
        e.symbol = Symbol;
        e.name = Name;
        e.atomicNumber = AtomicNumber;
        e.atomicMass = AtomicMass;
            
        // === Periodic Table ===
        e.group = Group;
        e.series = Series;
        e.period = Period;
        e.block = Block;
        e.standardState = StandardState;
        e.colour = Colour;
            
        // === Cosmic Abundance ===
        e.cosmicAbundance.unit = CosmicAbundance.unit;
        e.cosmicAbundance.value = CosmicAbundance.value;
            
        // === Electron Configuration ===
        e.electronConfiguration.display = ElectronConfiguration.display;
        e.electronConfiguration.nobleGas = ElectronConfiguration.nobleGas;
        for (const auto& orb : ElectronConfiguration.orbitals) {
            Interstellar::Data::OrbitalConfiguration destOrb;
            destOrb.orbital = orb.orbital;
            destOrb.electrons = orb.electrons;
            e.electronConfiguration.orbitals.push_back(destOrb);
        };
            
        // === Shells and Valence ===
        e.electronShells = ElectronShells;
        e.valenceElectrons = ValenceElectrons;
            
        // === Isotopes ===
        for (const auto& iso : Isotopes) {
            Interstellar::Data::ElementIsotope destIso;
            destIso.name             = iso.name;
            destIso.massNumber       = iso.massNumber;
            destIso.isStable         = iso.isStable;
            destIso.naturalAbundance = iso.naturalAbundance;
            destIso.halfLife         = iso.halfLife;
            destIso.radiationType    = iso.radiationType;

            e.isotopes.push_back(destIso);
        };

        // === Physical Properties ===
        //const auto& src = PhysicalProperties;
        //auto& dst = e.physicalProperties;

        //dst.density.value = src.Density.Value;
        //dst.density.unit = src.Density.Unit;

        //dst.meltingPoint.value = src.MeltingPoint.Value;
        //dst.meltingPoint.unit = src.MeltingPoint.Unit;

        //dst.boilingPoint.value = src.BoilingPoint.Value;
        //dst.boilingPoint.unit = src.BoilingPoint.Unit;

        //dst.triplePoint.temperature.value = src.TriplePoint.Temperature.Value;
        //dst.triplePoint.temperature.unit = src.TriplePoint.Temperature.Unit;
        //
        //dst.triplePoint.pressure.value = src.TriplePoint.Pressure.Value;
        //dst.triplePoint.pressure.unit = src.TriplePoint.Pressure.Unit;

        //dst.heatCapacity.value = src.HeatCapacity.Value;
        //dst.heatCapacity.unit = src.HeatCapacity.Unit;
        //
        //dst.thermalConductivity.value = src.ThermalConductivity.Value;
        //dst.thermalConductivity.unit = src.ThermalConductivity.Unit;
        //
        //dst.enthalpyOfFusion.value = src.EnthalpyOfFusion.Value;
        //dst.enthalpyOfFusion.unit = src.EnthalpyOfFusion.Unit;
        //
        //dst.enthalpyOfVaporization.value = src.EnthalpyOfVaporization.Value;
        //dst.enthalpyOfVaporization.unit = src.EnthalpyOfVaporization.Unit;
        //
        //dst.entropy.value = src.Entropy.Value;
        //dst.entropy.unit = src.Entropy.Unit;

        // === Phase Behavior ===
        //for (const auto& state : StateRanges) {
        //    Interstellar::Data::ElementData::StateRange destState;
        //    destState.temperature.min.value = state.Temperature.Min.Value;
        //    destState.temperature.min.unit = state.Temperature.Min.Unit;
        //    destState.temperature.max.value = state.Temperature.Max.Value;
        //    destState.temperature.max.unit = state.Temperature.Max.Unit;
        //    destState.pressure.min.value = state.Pressure.Min.Value;
        //    destState.pressure.min.unit = state.Pressure.Min.Unit;
        //    destState.pressure.max.value = state.Pressure.Max.Value;
        //    destState.pressure.max.unit = state.Pressure.Max.Unit;
        //    destState.state = state.State;

        //    e.stateRanges.push_back(destState);
        //};

        // === Chemical ===
        //e.chemical.electronegativity.value = Chemical.Electronegativity.Value;
        //e.chemical.electronegativity.unit = Chemical.Electronegativity.Unit;
        //e.chemical.ionizationEnergy.first.value = Chemical.IonizationEnergy.First.Value;
        //e.chemical.ionizationEnergy.first.unit = Chemical.IonizationEnergy.First.Unit;
        //e.chemical.electronAffinity.value = Chemical.ElectronAffinity.Value;
        //e.chemical.electronAffinity.unit = Chemical.ElectronAffinity.Unit;
        //for (const auto& oxy : Chemical.OxidationStates) {
        //    Interstellar::Data::ElementData::OxidationState destState;
        //    destState.state = oxy.State;
        //    destState.common = oxy.Common;
        //    e.chemical.oxidationStates.push_back(destState);
        //}
        //e.chemical.standardReductionPotential.value = Chemical.StandardReductionPotential.Value;
        //e.chemical.standardReductionPotential.unit = Chemical.StandardReductionPotential.Unit;
        //e.chemical.reactivity.value = Chemical.Reactivity.Value;
        //e.chemical.reactivity.scale = Chemical.Reactivity.Scale;
        //for (const auto& bond : Chemical.Bonding) {
        //    e.chemical.bonding.push_back(bond);
        //}
        //for (const auto& comp : Chemical.CommonCompounds) {
        //    e.chemical.commonCompounds.push_back(comp);
        //}
        //for (const auto& hyb : Chemical.HybridizationExamples) {
        //    e.chemical.hybridizationExamples.push_back(hyb);
        //}

        // === Quantum ===
        //e.quantum.atomicRadius.value = Quantum.AtomicRadius.Value;
        //e.quantum.atomicRadius.unit = Quantum.AtomicRadius.Unit;
        //e.quantum.covalentRadius.value = Quantum.CovalentRadius.Value;
        //e.quantum.covalentRadius.unit = Quantum.CovalentRadius.Unit;
        //e.quantum.vanDerWaalsRadius.value = Quantum.VanDerWaalsRadius.Value;
        //e.quantum.vanDerWaalsRadius.unit = Quantum.VanDerWaalsRadius.Unit;
        //e.quantum.magneticOrdering = Quantum.MagneticOrdering;
        //for (const auto& spin : Quantum.SpinStates) {
        //    e.quantum.spinStates.push_back(spin);
        //}
        //e.quantum.isParamagnetic = Quantum.IsParamagnetic;
        //e.quantum.nuclearSpin = Quantum.NuclearSpin;

        //
        //// === Environmental ===
        //e.environmentalProp.solubilityInWater.value = EnvironmentalProp.SolubilityInWater.Value;
        //e.environmentalProp.solubilityInWater.unit = EnvironmentalProp.SolubilityInWater.Unit;
        //e.environmentalProp.oxidationInAir.tendency = EnvironmentalProp.OxidationInAir.Tendency;
        //e.environmentalProp.oxidationInAir.spontaneous = EnvironmentalProp.OxidationInAir.Spontaneous;
        //e.environmentalProp.oxidationInAir.requiresSpark = EnvironmentalProp.OxidationInAir.RequiresSpark;
        //e.environmentalProp.toxicity = EnvironmentalProp.Toxicity;
        //e.environmentalProp.biologicalRole = EnvironmentalProp.BiologicalRole;
        //e.environmentalProp.hazards = EnvironmentalProp.Hazards;

        return e;
    };

    /**
    * @brief Parses a compact electron configuration string into structured orbitals.
    * 
    * @param input The string to parse (e.g., "[Ne] 3s2 3p6").
    * @return A structured electron configuration.
    */
    ElectronConfiguration ElementConfig::parseElectronConfiguration(const std::string& input) {
        Interstellar::Data::ElectronConfiguration config;
        config.nobleGas = "";  // Default if not found

        std::istringstream iss(input);
        std::string token;

        // Regex for orbital like "4f5", "6s2", etc.
        std::regex orbitalPattern(R"((\d+[spdf])(\d+))");

        while (iss >> token) {
            // Check if token is a noble gas (e.g., [Xe])
            if (token.front() == '[' && token.back() == ']') {
                config.nobleGas = token;
            }
            // Else, try to parse as orbital
            else {
                std::smatch match;
                if (std::regex_match(token, match, orbitalPattern)) {
                    OrbitalConfiguration orb;
                    orb.orbital = match[1].str();
                    orb.electrons = std::stoi(match[2].str());
                    config.orbitals.push_back(orb);
                }
                else {
                    s_Logger.LogWarn("Warning: Unrecognized token: {}",token);
                }
            }
        }

        return config;
    }


    /// @brief Returns the name of the JSON file to load.
    std::string ElementConfig::getFilename() const { return "elements_all.json"; }

}