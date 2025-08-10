#pragma once

#include "Interstellar/Config/JsonConfigBase.hpp"
#include "Interstellar/Data/ElementData.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <regex>
#include <iostream>
#include <optional>
#include "Interstellar/Utils/String/StringUtils.hpp"
#include "Interstellar/Core/Logging.hpp"

/**
 * @file ElementConfig.hpp
 * @ingroup InterstellarConfig
 * @brief Extended element metadata configuration loaded from JSON.
 */
namespace Interstellar::Config::JsonImpl {

    /**
     * @struct IntUnit
     * @ingroup InterstellarConfig
     * @brief Represents an optional integer value with a unit.
     */
    struct IntUnit {
        std::optional<int> Value;      ///< Optional integer value.
        std::optional<std::string> Unit; ///< Unit of measurement (e.g., "pm").
    };

    /**
     * @struct FloatUnit
     * @ingroup InterstellarConfig
     * @brief Represents an optional float value with a unit.
     */
    struct FloatUnit {
        std::optional<float> Value;      ///< Optional float value.
        std::optional<std::string> Unit; ///< Unit of measurement.
    };

    /**
     * @struct RangeEndpoint
     * @ingroup InterstellarConfig
     * @brief Represents a range endpoint with a value and unit.
     */
    struct RangeEndpoint {
        float Value = 0.0f;
        std::string Unit;
    };

    /**
     * @struct Range
     * @ingroup InterstellarConfig
     * @brief Represents a numerical range with min and max endpoints.
     */
    struct Range {
        RangeEndpoint Min;
        RangeEndpoint Max;
    };

    /**
     * @struct StateRange
     * @ingroup InterstellarConfig
     * @brief Represents an element state at specific temperature and pressure ranges.
     */
    struct StateRange {
        Range Temperature;
        Range Pressure;
        std::string State;
    };

    /**
     * @struct ScalarProperty
     * @ingroup InterstellarConfig
     * @brief Represents a scalar value with optional float and unit.
     */
    struct ScalarProperty {
        std::optional<float> Value;
        std::string Unit;
    };

    /**
     * @struct IonizationEnergy
     * @ingroup InterstellarConfig
     * @brief Contains first ionization energy data.
     */
    struct IonizationEnergy {
        ScalarProperty First; ///< First ionization energy.
    };

    /**
     * @struct OxidationState
     * @ingroup InterstellarConfig
     * @brief Represents a single oxidation state.
     */
    struct OxidationState {
        int State = 0; ///< Oxidation number.
        bool Common = false; ///< Indicates if it's commonly observed.
    };

    /**
     * @struct Reactivity
     * @ingroup InterstellarConfig
     * @brief Represents reactivity score and its scale.
     */
    struct Reactivity {
        float Value = 0.0f; ///< Reactivity value.
        std::string Scale;  ///< Measurement scale.
    };

    /**
     * @struct ChemicalProperties
     * @ingroup InterstellarConfig
     * @brief Group of properties describing chemical behavior.
     */
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

    /**
     * @struct QuantumProperties
     * @ingroup InterstellarConfig
     * @brief Describes quantum mechanical traits of an element.
     */
    struct QuantumProperties {
        IntUnit AtomicRadius;
        IntUnit CovalentRadius;
        IntUnit VanDerWaalsRadius;
        std::string MagneticOrdering;
        std::vector<float> SpinStates;
        bool IsParamagnetic = false;
        float NuclearSpin = 0.0f;
    };

    /**
     * @struct EnvironmentalSolubility
     * @ingroup InterstellarConfig
     * @brief Describes solubility in environmental context.
     */
    struct EnvironmentalSolubility {
        std::optional<float> Value;
        std::optional<std::string> Unit;
    };

    /**
     * @struct OxidationInAirInfo
     * @ingroup InterstellarConfig
     * @brief Describes behavior of element in air.
     */
    struct OxidationInAirInfo {
        std::string Tendency;
        bool Spontaneous = false;
        bool RequiresSpark = false;
    };

    /**
     * @struct EnvironmentalProperties
     * @ingroup InterstellarConfig
     * @brief Environmental reactivity and toxicity data.
     */
    struct EnvironmentalProperties {
        EnvironmentalSolubility SolubilityInWater;
        OxidationInAirInfo OxidationInAir;
        std::string Toxicity;
        std::string BiologicalRole;
        std::vector<std::string> Hazards;
    };

    /**
     * @class ElementConfig
     * @ingroup InterstellarConfig
     * @brief Parses JSON-based configuration into ElementData structures.
     *
     * This class reads and interprets extended periodic table data from JSON files.
     * It acts as a bridge between raw configuration and the in-game runtime data model
     * used for rendering and simulation logic.
     *
     * Supports deserialization of electron configuration, isotopes, physical, chemical,
     * quantum, and environmental properties.
     *
     * @since 1.0
     */
    class ElementConfig : public JsonConfigBase {
    public:

        /**
         * @brief Constructs a new ElementConfig object and registers JSON fields.
         */
        ElementConfig();

        /**
         * @brief Converts this configuration to the runtime-safe ElementData model.
         * @return A fully populated ElementData structure.
         */
        [[nodiscard]] Interstellar::Data::ElementData toData() const;

        /**
         * @brief Parses a formatted string like "[Ne] 3s2 3p6" into a structured electron config.
         * @param input Human-readable electron configuration.
         * @return Parsed configuration structure.
         */
        Interstellar::Data::ElectronConfiguration parseElectronConfiguration(const std::string& input);

        /// @brief Returns the top-level namespace used in JSON.
        std::string getNamespace() const override { return "data"; }

        /// @brief Returns the name of the JSON file to load.
        std::string getFilename() const override;

    private:
        /**
        * @brief Registers all fields to enable automatic JSON (de)serialization.
        */
        void registerAllFields();

        // === Identification ===
        std::string Symbol;             ///< Atomic symbol (e.g. "H").
        std::string Name;               ///< Full element name (e.g. "Hydrogen").
        int AtomicNumber = 0;           ///< Number of protons.
        float AtomicMass = 0.f;         ///< Atomic mass (unified atomic mass units).

         //=== Periodic Table ===
        std::optional<int> Group;       ///< Periodic table group number - null if not applicable.
        std::optional<Interstellar::Data::ElementSeries> Series; // in ElementData

        int Period = 0;                 ///< Periodic table period number.
        std::string Block;              ///< Block classification (s, p, d, f).
        std::string StandardState;      ///< State under standard conditions.

        glm::vec4 Colour = { 1.f, 1.f, 1.f, 1.f }; ///< Display color (used for visualizations).

        // === Scientific Properties ===
        Interstellar::Data::CosmicAbundance CosmicAbundance; ///< Abundance in the universe.
        Interstellar::Data::ElectronConfiguration ElectronConfiguration; ///< Electron shell structure.
        std::vector<int> ElectronShells; ///< Population of each shell.
        int ValenceElectrons = 0; ///< Number of valence electrons.
        std::vector<Interstellar::Data::ElementIsotope> Isotopes; ///< List of isotopic data.
        Interstellar::Data::PhysicalProperties PhysicalProperties; ///< Physical constants.
        //std::vector<StateRange> StateRanges; ///< Phase behavior by temperature/pressure.
        //ChemicalProperties Chemical; ///< Reactivity, bonds, oxidation, etc.
        //QuantumProperties Quantum; ///< Radius, magnetism, spins.
        //EnvironmentalProperties EnvironmentalProp; ///< Behavior in nature and bio systems.
    };

}
