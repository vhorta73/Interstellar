#pragma once

#include <string>
#include <vector>
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
     * @brief Defines static and extended physical properties of a chemical element.
     *
     * Can be used in simulation, chemistry, planetary formation, and rendering systems.
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
        std::vector<std::string> commonCompounds;

        // === Utility ===
        bool isValid() const {
            return !symbol.empty() && atomicNumber > 0 && atomicMass > 0.0f;
        }

        /**
         * @brief Get the state of the element based on temperature (K) and pressure (atm).
         */
        ElementState getState(float temperatureK, float pressureAtm = 1.0f) const {
            if (meltingPoint <= 0.f || boilingPoint <= 0.f)
                return ElementState::Unknown;

            // Approximate shift in boiling point under pressure
            float adjustedBoiling = boilingPoint * std::pow(pressureAtm, 0.1f);
            float adjustedMelting = meltingPoint * std::pow(pressureAtm, 0.02f);

            if (temperatureK < adjustedMelting )
                return ElementState::Solid;

            if (temperatureK < adjustedBoiling)
                return ElementState::Liquid;

            return (temperatureK >= adjustedBoiling && temperatureK >= adjustedMelting
                && temperatureK >= criticalPointTemp
                && pressureAtm > criticalPointPressure)
                ? ElementState::Plasma // Plasma if above critical point
                : ElementState::Gas;
        }

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
        std::string lower = s;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "solid")   return ElementState::Solid;
        if (lower == "liquid")  return ElementState::Liquid;
        if (lower == "gas")     return ElementState::Gas;
        if (lower == "plasma")  return ElementState::Plasma;
        return ElementState::Unknown;
    }

    /// @brief Stream output for ElementState
    inline std::ostream& operator<<(std::ostream& os, ElementState state) {
        return os << to_string(state);
    }


}