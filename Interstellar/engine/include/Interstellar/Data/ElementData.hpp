#pragma once

#include <string>
#include <vector>
#include <locale>
#include <algorithm>
#include <ostream>
#include <glm/glm.hpp>

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

        // === Physical Properties ===
        float atomicMass = 0.0f;   ///< Mass of an atom in atomic mass units (u).
        float density = 0.0f;      ///< Density in g/cm^3.
        float meltingPoint = 0.0f; ///< Melting point in Kelvin.
        float boilingPoint = 0.0f; ///< Boiling point in Kelvin.
        float atomicRadius = 0.0f; ///< Atomic radius in picometers.
        float electronegativity = 0.0f; ///< Electronegativity on Pauling scale.
        ElementState stateAtSTP = ElementState::Unknown;

        float triplePointTemp = 0.0f;
        float triplePointPressure = 0.0f;
        float criticalPointTemp = 0.0f;
        float criticalPointPressure = 0.0f;

        // === Environmental & Chemical ===
        float abundance = 0.0f;    ///< Crustal abundance in ppm.
        bool isMetal = false;
        bool isToxic = false;
        bool isMagnetic = false;

        // === Rendering & Simulation ===
        glm::vec3 displayColor = { 1.f, 1.f, 1.f };
        float reactivity = 0.0f;   ///< Reactivity score (0 = inert, 1 = very reactive).
        float radiation = 0.0f;    ///< Radiation level (0 = non-radioactive).

        // === Metadata ===
        std::vector<std::string> commonCompounds; ///< Examples: "H2O", "CO2", "NaCl"

        /// @brief Checks if the element has valid essential properties.
        /// @return True if the element has valid symbol, atomic number, and mass.
        [[nodiscard]] bool isValid() const noexcept {
            return !symbol.empty() && atomicNumber > 0 && atomicMass > 0.0f;
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
            if (meltingPoint <= 0.f || boilingPoint <= 0.f)
                return ElementState::Unknown;

            // Approximate shift in boiling point under pressure
            float adjustedBoiling = boilingPoint * std::pow(pressureAtm, 0.1f);
            float adjustedMelting = meltingPoint * std::pow(pressureAtm, 0.02f);

            if (temperatureK < adjustedMelting)
                return ElementState::Solid;

            if (temperatureK < adjustedBoiling)
                return ElementState::Liquid;

            return (temperatureK >= criticalPointTemp
                && pressureAtm > criticalPointPressure)
                ? ElementState::Gas
                : ElementState::Plasma; // Plasma if above critical point
        }

        /// @brief Compares all fields for equality.
        bool operator==(const ElementData& other) const = default;
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