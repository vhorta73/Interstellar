#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <optional>
//#include <iosfwd>
#include <locale>
#include <algorithm>

// mp-units: just tpes in headers.
//#include <mp-units/quantity.h>
//#include <mp-units/systems/si/units.h>

#include <glm/vec4.hpp>

#include "Interstellar/Utils/String/StringUtils.hpp"
#include "Interstellar/Utils/Time/TimeUtils.hpp"

//namespace si = mp_units:si;

namespace Interstellar::Data {

    /**
     * @ingroup InterstellarData
     * @brief Enumerates physical states of elements at standard temperature and pressure.
     *
     * @since 1.0
     */
    enum class ElementState {
        Solid,
        Liquid,
        Gas,
        Plasma,
        Unknown
    };

    /**
     * @ingroup InterstellarData
     * @brief Enumerates the element series.
     *
     * @since 1.0
     */
    enum class ElementSeries {
        Actinide,
        AlkaliMetal,
        AlkalineEarthMetal,
        Halogen,
        Lanthanide,
        Metalloid,
        NobleGas,
        Nonmetal,
        PostTransitionMetal,
        TransitionMetal,
        Unknown
    };

    /**
     * @struct CosmicAbundance
     * @ingroup InterstellarConfig
     * @brief Stores cosmic abundance data for an element.
     */
    struct CosmicAbundance {
        std::optional<float> value;
        std::string unit;
    };

    /**
     * @struct OrbitalConfiguration
     * @ingroup InterstellarConfig
     * @brief Represents an orbital and its electron count.
     */
    struct OrbitalConfiguration {
        std::string orbital; ///< Orbital name (e.g., "2p").
        int electrons = 0;   ///< Number of electrons in the orbital.
    };

    /**
     * @struct ElectronConfiguration
     * @ingroup InterstellarConfig
     * @brief Parsed electron configuration data used in atomic modeling.
     */
    struct ElectronConfiguration {
        std::string nobleGas; ///< Noble gas shorthand notation (e.g., "[Ne]").
        std::string display; ///< Original configuration string.
        std::vector<OrbitalConfiguration> orbitals; ///< Parsed orbitals.
    };

    /**
    * @struct ElementIsotope
    * @ingroup InterstellarConfig
    * @brief Describes an isotope of a chemical element.
    */
    struct ElementIsotope {
        std::string name; ///< Isotope name.
        float massNumber = 0.0f; ///< Mass number.
        bool isStable = false; ///< Stability flag.
        float naturalAbundance = 0.0f; ///< Abundance percentage.
        std::optional<Interstellar::Utils::Time::TimeValueUnit> halfLife; ///< Half-life in seconds.
        std::optional<std::string> radiationType; ///< Type of radiation emitted, if any.
    };


    /**
     * @struct PropertyWithUnit
     * @ingroup InterstellarConfig
     * @brief Represents a physical property with an optional value and unit.
     */
    struct PropertyWithUnit {
        std::optional<float> Value; ///< Property value.
        std::string Unit; ///< Unit of the property.
    };

    /**
     * @struct TriplePoint
     * @ingroup InterstellarConfig
     * @brief Describes the triple point of a substance.
     */
    struct TriplePoint {
        PropertyWithUnit Temperature; ///< Triple point temperature.
        PropertyWithUnit Pressure;    ///< Triple point pressure.
    };

    /**
    * @struct PhysicalProperties
    * @ingroup InterstellarConfig
    * @brief Groups all physical properties of an element.
    */
    struct PhysicalProperties {
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

    /**
     * @ingroup InterstellarData
     * @brief Holds physical, chemical, and rendering metadata for a single chemical element.
     *
     * Used in multiple subsystems including chemistry, simulation, and visualization.
     * This is a lightweight, POD-like structure for runtime queries and rendering context.
     *
     * @since 1.0
     */
    struct ElementData {
        // === Identification ===
        std::string symbol;        ///< Atomic symbol (e.g., "H", "Fe").
        std::string name;          ///< Full name (e.g., "Hydrogen", "Iron").
        int atomicNumber = 0;      ///< Atomic number (proton count).
        float atomicMass = 0.0f;   ///< Mass of an atom in atomic mass units (u).

        // Position
        std::optional<int> group;
        std::optional<ElementSeries> series; ///< Periodic table series (e.g., NobleGas, TransitionMetal).
        int period = 0;
        std::string block;
        std::string standardState;
 
        // Rendering
        glm::vec4 colour = { 1.f, 1.f, 1.f, 1.f };

        CosmicAbundance cosmicAbundance;
        ElectronConfiguration electronConfiguration;
        std::vector<int> electronShells;
        int valenceElectrons = 0;
        std::vector<ElementIsotope> isotopes;

        //struct FloatUnit {
        //    std::optional<float> value;
        //    std::optional<std::string> unit;
        //};
        //struct IntUnit {
        //    std::optional<int> value;
        //    std::optional<std::string> unit;
        //};

        //struct PropertyWithUnit {
        //    std::optional<float> value;
        //    std::string unit;
        //};

        //struct TriplePoint {
        //    PropertyWithUnit temperature;
        //    PropertyWithUnit pressure;
        //};

        //struct PhysicalPropertiesBlock {
        //    PropertyWithUnit density;
        //    PropertyWithUnit meltingPoint;
        //    PropertyWithUnit boilingPoint;
        //    TriplePoint triplePoint;
        //    PropertyWithUnit heatCapacity;
        //    PropertyWithUnit thermalConductivity;
        //    PropertyWithUnit enthalpyOfFusion;
        //    PropertyWithUnit enthalpyOfVaporization;
        //    PropertyWithUnit entropy;
        //};
        ////PhysicalPropertiesBlock physicalProperties;

        //struct RangeEndpoint {
        //    float value = 0.0f;
        //    std::string unit;
        //};

        //struct Range {
        //    RangeEndpoint min;
        //    RangeEndpoint max;
        //};

        //struct StateRange {
        //    Range temperature;
        //    Range pressure;
        //    std::string state;
        //};
        ////std::vector<StateRange> stateRanges;

        //struct ScalarProperty {
        //    std::optional<float> value;
        //    std::string unit;
        //};

        //struct IonizationEnergy {
        //    ScalarProperty first;
        //    // Extendable: Second, Third, etc.
        //};

        //struct OxidationState {
        //    int state = 0;
        //    bool common = false;
        //};

        //struct Reactivity {
        //    float value = 0.0f;
        //    std::string scale;
        //};

        //struct ChemicalProperties {
        //    ScalarProperty electronegativity;
        //    IonizationEnergy ionizationEnergy;
        //    ScalarProperty electronAffinity;
        //    ScalarProperty standardReductionPotential;
        //    Reactivity reactivity;
        //    std::vector<OxidationState> oxidationStates;
        //    std::vector<std::string> bonding;
        //    std::vector<std::string> commonCompounds;
        //    std::vector<std::pair<std::string, std::string>> hybridizationExamples;
        //};
        ////ChemicalProperties chemical;

        //struct QuantumProperties {
        //    IntUnit atomicRadius;
        //    IntUnit covalentRadius;
        //    IntUnit vanDerWaalsRadius;
        //    std::string magneticOrdering;
        //    std::vector<float> spinStates;
        //    bool isParamagnetic = false;
        //    float nuclearSpin = 0.0f;
        //};
        ////QuantumProperties quantum;

        //struct EnvironmentalSolubility {
        //    std::optional<float> value;
        //    std::optional<std::string> unit;
        //};

        //struct OxidationInAirInfo {
        //    std::string tendency;
        //    bool spontaneous = false;
        //    bool requiresSpark = false; // Optional
        //};

        //struct EnvironmentalProperties {
        //    EnvironmentalSolubility solubilityInWater;
        //    OxidationInAirInfo oxidationInAir;
        //    std::string toxicity;
        //    std::string biologicalRole;
        //    std::vector<std::string> hazards;
        //};
        //EnvironmentalProperties environmentalProp;

        // --- Runtime ---
        /// @brief Checks if the element has valid essential properties.
        /// @return True if the element has valid symbol, atomic number, and mass.
        [[nodiscard]] bool isValid() const noexcept {
            return !symbol.empty();//&& atomicNumber > 0;
        }

        /**
         * @brief Computes the physical state of the element at a given temperature and pressure.
         *
         * This approximation considers pressure influence on melting and boiling points
         * and returns Plasma if supercritical conditions are met.
         *
         * @param temperatureK Temperature in Kelvin
         * @param pressureAtm Pressure in atmospheres (default = 1.0)
         * @return ElementState Resulting state under given conditions
         *
         * @since 1.0
         */
        //[[nodiscard]] ElementState getState(float temperatureK, float pressureAtm = 1.0f) const noexcept {
            //for (const auto& range : stateRanges) {
                //if (range.temperatureRange.contains(temperatureK) &&
                    //range.pressureRange.contains(pressureAtm)) {
                    //return range.state;
                //}
            //}
            //return ElementState::Unknown;
        //}

        /// @brief Compares all fields for equality.
        bool operator==(const ElementData& other) const = default;

        //[[nodiscard]] std::string GetString() {
            //return std::string("Element: ") + name + " (" + symbol + "), Atomic Number: " + std::to_string(atomicNumber) +
                //", Atomic Mass: " + std::to_string(atomicMass) + ", Group: " + std::to_string(group) +
                //", Period: " + std::to_string(period) + ", Block: " + block;

        //}
        
        };

    /// @brief Convert ElementState to lowercase string
    inline constexpr const char* to_string(ElementState state) {
        switch (state) {
            case ElementState::Solid:   return "solid";
            case ElementState::Liquid:  return "liquid";
            case ElementState::Gas:     return "gas";
            case ElementState::Plasma:  return "plasma";
            default:                    return "unknown";
           }
        }

    /// @brief Convert string to ElementState (case-insensitive)
    inline constexpr ElementState element_state_from_string(std::string_view s) {
        auto lower = Interstellar::Utils::String::to_lower_copy(std::string(s));

        if (lower == "solid")  return ElementState::Solid;
        if (lower == "liquid") return ElementState::Liquid;
        if (lower == "gas")    return ElementState::Gas;
        if (lower == "plasma") return ElementState::Plasma;
        return ElementState::Unknown; // fallback/default
    }

    /// @brief Convert string to ElementSeries (case-insensitive)
    inline ElementSeries string_to_element_series(std::string_view s) {
        auto lower = Interstellar::Utils::String::to_lower_copy(std::string(s));

        if (lower == "lanthanide")            return ElementSeries::Lanthanide;
        if (lower == "actinide")              return ElementSeries::Actinide;
        if (lower == "alkalimetal")           return ElementSeries::AlkaliMetal;
        if (lower == "alkalineearthmetal")    return ElementSeries::AlkalineEarthMetal;
        if (lower == "transitionmetal")       return ElementSeries::TransitionMetal;
        if (lower == "posttransitionmetal")   return ElementSeries::PostTransitionMetal;
        if (lower == "metalloid")             return ElementSeries::Metalloid;
        if (lower == "nonmetal")              return ElementSeries::Nonmetal;
        if (lower == "halogen")               return ElementSeries::Halogen;
        if (lower == "noblegas")              return ElementSeries::NobleGas;

        return ElementSeries::Unknown;
    }


    inline constexpr const std::string element_series_to_string(ElementSeries series) {
        switch (series) {
        case ElementSeries::Lanthanide:          return "lanthanide";
        case ElementSeries::Actinide:            return "actinide";
        case ElementSeries::AlkaliMetal:         return "alkali metal";
        case ElementSeries::AlkalineEarthMetal:  return "alkaline earth metal";
        case ElementSeries::TransitionMetal:     return "transition metal";
        case ElementSeries::PostTransitionMetal: return "post-transition metal";
        case ElementSeries::Metalloid:           return "metalloid";
        case ElementSeries::Nonmetal:            return "nonmetal";
        case ElementSeries::Halogen:             return "halogen";
        case ElementSeries::NobleGas:            return "noble gas";
        default:                                 return "unknown";
        }
    }

    /// @ingroup InterstellarData
    /// @brief Outputs a human-readable string for the given ElementState.
    inline std::ostream& operator<<(std::ostream& os, ElementState state) {
        return os << to_string(state);
    }
}