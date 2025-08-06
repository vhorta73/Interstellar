#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <optional>
#include <ostream>
#include <locale>
#include <algorithm>

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
        int group = 0;
        int period = 0;
        std::string block;

        std::string standardState;
 
        // Rendering
        glm::vec3 colour = { 1.f, 1.f, 1.f };

        struct FloatUnit {
            std::optional<float> value;
            std::optional<std::string> unit;
        };
        struct IntUnit {
            std::optional<int> value;
            std::optional<std::string> unit;
        };

        struct CosmicAbundance {
          float value = 0.0f;
          std::string unit;
        } cosmicAbundance;

        struct ElectronConfiguration {
            std::string nobleGas;
            std::string display;

            struct Orbital {
                std::string orbital;
                int electrons = 0;
            };

            std::vector<Orbital> orbitals;
        } electronConfiguration;

        std::vector<int> electronShells;
        int valenceElectrons = 0;
        struct Isotope {
            std::string name;
            float massNumber = 0.0f;
            bool isStable = false;
            float naturalAbundance = 0.0f;
            std::optional<float> halfLife;
            std::optional<std::string> radiationType;
        };
        std::vector<Isotope> isotopes;

        struct PropertyWithUnit {
            std::optional<float> value;
            std::string unit;
        };

        struct TriplePoint {
            PropertyWithUnit temperature;
            PropertyWithUnit pressure;
        };

        struct PhysicalPropertiesBlock {
            PropertyWithUnit density;
            PropertyWithUnit meltingPoint;
            PropertyWithUnit boilingPoint;
            TriplePoint triplePoint;
            PropertyWithUnit heatCapacity;
            PropertyWithUnit thermalConductivity;
            PropertyWithUnit enthalpyOfFusion;
            PropertyWithUnit enthalpyOfVaporization;
            PropertyWithUnit entropy;
        };
        PhysicalPropertiesBlock physicalProperties;

        struct RangeEndpoint {
            float value = 0.0f;
            std::string unit;
        };

        struct Range {
            RangeEndpoint min;
            RangeEndpoint max;
        };

        struct StateRange {
            Range temperature;
            Range pressure;
            std::string state;
        };
        std::vector<StateRange> stateRanges;

        struct ScalarProperty {
            std::optional<float> value;
            std::string unit;
        };

        struct IonizationEnergy {
            ScalarProperty first;
            // Extendable: Second, Third, etc.
        };

        struct OxidationState {
            int state = 0;
            bool common = false;
        };

        struct Reactivity {
            float value = 0.0f;
            std::string scale;
        };

        struct ChemicalProperties {
            ScalarProperty electronegativity;
            IonizationEnergy ionizationEnergy;
            ScalarProperty electronAffinity;
            ScalarProperty standardReductionPotential;
            Reactivity reactivity;
            std::vector<OxidationState> oxidationStates;
            std::vector<std::string> bonding;
            std::vector<std::string> commonCompounds;
            std::vector<std::pair<std::string, std::string>> hybridizationExamples;
        };
        ChemicalProperties chemical;

        struct QuantumProperties {
            IntUnit atomicRadius;
            IntUnit covalentRadius;
            IntUnit vanDerWaalsRadius;
            std::string magneticOrdering;
            std::vector<float> spinStates;
            bool isParamagnetic = false;
            float nuclearSpin = 0.0f;
        };
        QuantumProperties quantum;

        struct EnvironmentalSolubility {
            std::optional<float> value;
            std::optional<std::string> unit;
        };

        struct OxidationInAirInfo {
            std::string tendency;
            bool spontaneous = false;
            bool requiresSpark = false; // Optional
        };

        struct EnvironmentalProperties {
            EnvironmentalSolubility solubilityInWater;
            OxidationInAirInfo oxidationInAir;
            std::string toxicity;
            std::string biologicalRole;
            std::vector<std::string> hazards;
        };
        EnvironmentalProperties environmentalProp;

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
        [[nodiscard]] ElementState getState(float temperatureK, float pressureAtm = 1.0f) const noexcept {
            //for (const auto& range : stateRanges) {
                //if (range.temperatureRange.contains(temperatureK) &&
                    //range.pressureRange.contains(pressureAtm)) {
                    //return range.state;
                //}
            //}
            return ElementState::Unknown;
        }

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
    inline ElementState parseState(const std::string& s) {
        std::string lower = s; // Create a mutable copy.

        std::locale loc;
        std::transform(lower.begin(), lower.end(), lower.begin(),
            [&loc](char c) { return std::tolower(c, loc); });

        if (lower == "solid")   return ElementState::Solid;
        if (lower == "liquid")  return ElementState::Liquid;
        if (lower == "gas")     return ElementState::Gas;
        if (lower == "plasma")  return ElementState::Plasma;
        return ElementState::Unknown;
    }

    /// @ingroup InterstellarData
    /// @brief Outputs a human-readable string for the given ElementState.
    inline std::ostream& operator<<(std::ostream& os, ElementState state) {
        return os << to_string(state);
    }
}