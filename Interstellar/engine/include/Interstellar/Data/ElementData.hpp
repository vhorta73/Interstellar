#pragma once

#include <string>
#include <algorithm>
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
     * @brief Defines static properties of a chemical element used in simulation.
     *
     * Can be extended for fusion, bonding, or rendering logic.
     *
     * @since 1.0
     */
    struct ElementData {
        // Identification
        std::string symbol;       ///< Atomic symbol (e.g., "H", "Fe").
        std::string name;         ///< Full name (e.g., "Hydrogen", "Iron").
        int atomicNumber = 0;     ///< Atomic number (proton count).

        // Physical Properties
        float atomicMass = 0.0f;  ///< Mass of a single atom in atomic mass units (u).
        float density = 0.0f;     ///< Density in g/cm^3.
        ElementState stateAtSTP = ElementState::Unknown; ///< Physical state at STP.

        // Display & Simulation
        glm::vec3 displayColor = { 1.f, 1.f, 1.f }; ///< RGB color used in UI/visualizations.
        float reactivity = 0.0f;  ///< Reactivity score (0 = inert).
        float radiation = 0.0f;   ///< Radiation level (0 = non-radioactive).

        bool isValid() const {
            return !symbol.empty() && atomicNumber > 0 && atomicMass > 0.0f;
        }

        bool operator==(const ElementData& other) const = default;
    };

    inline constexpr const char* to_string(ElementState state) {
        switch (state) {
            case ElementState::Solid:   return "solid";
            case ElementState::Liquid:  return "liquid";
            case ElementState::Gas:     return "gas";
            case ElementState::Plasma:  return "plasma";
            default:                    return "unknown";
        }
    }

    inline ElementState parseState(const std::string& s) {
        std::string lower = s;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "solid")   return ElementState::Solid;
        if (lower == "liquid")  return ElementState::Liquid;
        if (lower == "gas")     return ElementState::Gas;
        if (lower == "plasma")  return ElementState::Plasma;
        return ElementState::Unknown;
    };

    inline std::ostream& operator<<(std::ostream& os, ElementState state) {
        return os << std::string(to_string(state));
    }


}