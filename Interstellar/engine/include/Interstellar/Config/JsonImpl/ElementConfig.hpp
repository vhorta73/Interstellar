#pragma once

#include "Interstellar/Config/JsonConfigBase.hpp"
#include "Interstellar/Data/ElementData.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Interstellar::Config {
    //using Interstellar::Data::parseState;
    //using Interstellar::Data::ElementData;

    class ElementConfig : public JsonConfigBase {
    public:
        // --- Fields ---
        std::string Symbol;
        std::string Name;
        int         AtomicNumber = 0;
        float       AtomicMass = 0.f;
        float       Density = 0.f;
        std::string StateAtSTP;
        glm::vec3   DisplayColor = { 1.f, 1.f, 1.f };
        float       Reactivity = 0.f;
        float       Radiation = 0.f;

        ElementConfig() {
            registerField("Symbol", [this]() { return Symbol; },
                [this](auto& v) { Symbol = v.get<std::string>(); });

            registerField("Name", [this]() { return Name; },
                [this](auto& v) { Name = v.get<std::string>(); });

            registerField("AtomicNumber", [this]() { return AtomicNumber; },
                [this](auto& v) { AtomicNumber = v.get<int>(); });

            registerField("AtomicMass", [this]() { return AtomicMass; },
                [this](auto& v) { AtomicMass = v.get<float>(); });

            registerField("Density", [this]() { return Density; },
                [this](auto& v) { Density = v.get<float>(); });

            registerField("StateAtSTP", [this]() { return StateAtSTP; },
                [this](auto& v) { StateAtSTP = v.get<std::string>(); });

            registerField("DisplayColor", [this]() {
                return std::vector<float>{ DisplayColor.r, DisplayColor.g, DisplayColor.b };
                },
                [this](auto& v) {
                    auto vec = v.get<std::vector<float>>();
                    if (vec.size() == 3)
                        DisplayColor = glm::vec3(vec[0], vec[1], vec[2]);
                });

            registerField("Reactivity", [this]() { return Reactivity; },
                [this](auto& v) { Reactivity = v.get<float>(); });

            registerField("Radiation", [this]() { return Radiation; },
                [this](auto& v) { Radiation = v.get<float>(); });
        }

        /**
         * @brief Converts the loaded config into a runtime-safe ElementData instance.
         */
        [[nodiscard]] Interstellar::Data::ElementData toData() const {
            auto state = Interstellar::Data::parseState(StateAtSTP);
            return {
                Symbol,
                Name,
                AtomicNumber,
                AtomicMass,
                Density,
                state,
                DisplayColor,
                Reactivity,
                Radiation
            };
        }

        std::string getNamespace() const override { return "data"; }
        std::string getFilename()  const override { return "elements.json"; }
    };

}
