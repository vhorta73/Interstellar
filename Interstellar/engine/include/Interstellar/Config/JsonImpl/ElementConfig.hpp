#pragma once

#include "Interstellar/Config/JsonConfigBase.hpp"
#include "Interstellar/Data/ElementData.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Interstellar::Config {

    class ElementConfig : public JsonConfigBase {
    public:
        // Basic Identification
        std::string Symbol;
        std::string Name;
        int         AtomicNumber = 0;

        // Physical
        float AtomicMass = 0.f;
        float Density = 0.f;
        float MeltingPoint = 0.f;
        float BoilingPoint = 0.f;
        float AtomicRadius = 0.f;
        float Electronegativity = 0.f;
        std::string StateAtSTP;

        float TriplePointTemp = 0.0f;
        float TriplePointPressure = 0.0f;
        float CriticalPointTemp = 0.0f;
        float CriticalPointPressure = 0.0f;

        // Environmental
        float Abundance = 0.f;
        bool IsMetal = false;
        bool IsToxic = false;
        bool IsMagnetic = false;

        // Visual / Simulation
        glm::vec3 DisplayColor = { 1.f, 1.f, 1.f };
        float Reactivity = 0.f;
        float Radiation = 0.f;

        std::vector<std::string> CommonCompounds;

        ElementConfig() {
            using json = nlohmann::json;

            // === Registration ===
            registerField("Symbol", [this]() { return Symbol; },
                [this](const json& v) { Symbol = v.get<std::string>(); });

            registerField("Name", [this]() { return Name; },
                [this](const json& v) { Name = v.get<std::string>(); });

            registerField("AtomicNumber", [this]() { return AtomicNumber; },
                [this](const json& v) { AtomicNumber = v.get<int>(); });

            registerField("AtomicMass", [this]() { return AtomicMass; },
                [this](const json& v) { AtomicMass = v.get<float>(); });

            registerField("Density", [this]() { return Density; },
                [this](const json& v) { Density = v.get<float>(); });

            registerField("MeltingPoint", [this]() { return MeltingPoint; },
                [this](const json& v) { MeltingPoint = v.get<float>(); });

            registerField("BoilingPoint", [this]() { return BoilingPoint; },
                [this](const json& v) { BoilingPoint = v.get<float>(); });

            registerField("AtomicRadius", [this]() { return AtomicRadius; },
                [this](const json& v) { AtomicRadius = v.get<float>(); });

            registerField("Electronegativity", [this]() { return Electronegativity; },
                [this](const json& v) { Electronegativity = v.get<float>(); });

            registerField("StateAtSTP", [this]() { return StateAtSTP; },
                [this](const json& v) { StateAtSTP = v.get<std::string>(); });

            registerField("Abundance", [this]() { return Abundance; },
                [this](const json& v) { Abundance = v.get<float>(); });

            registerField("IsMetal", [this]() { return IsMetal; },
                [this](const json& v) { IsMetal = v.get<bool>(); });

            registerField("IsToxic", [this]() { return IsToxic; },
                [this](const json& v) { IsToxic = v.get<bool>(); });

            registerField("IsMagnetic", [this]() { return IsMagnetic; },
                [this](const json& v) { IsMagnetic = v.get<bool>(); });

            registerField("DisplayColor", [this]() {
                return std::vector<float>{ DisplayColor.r, DisplayColor.g, DisplayColor.b };
                },
                [this](const json& v) {
                    auto vec = v.get<std::vector<float>>();
                    if (vec.size() == 3)
                        DisplayColor = glm::vec3(vec[0], vec[1], vec[2]);
                });

            registerField("Reactivity", [this]() { return Reactivity; },
                [this](const json& v) { Reactivity = v.get<float>(); });

            registerField("Radiation", [this]() { return Radiation; },
                [this](const json& v) { Radiation = v.get<float>(); });

            registerField("CommonCompounds", [this]() { return CommonCompounds; },
                [this](const json& v) { CommonCompounds = v.get<std::vector<std::string>>(); });

            registerField("TriplePointTemp", [this]() { return TriplePointTemp; },
                [this](const json& v) { TriplePointTemp = v.get<float>(); });

            registerField("TriplePointPressure", [this]() { return TriplePointPressure; },
                [this](const json& v) { TriplePointPressure = v.get<float>(); });

            registerField("CriticalPointTemp", [this]() { return CriticalPointTemp; },
                [this](const json& v) { CriticalPointTemp = v.get<float>(); });

            registerField("CriticalPointPressure", [this]() { return CriticalPointPressure; },
                [this](const json& v) { CriticalPointPressure = v.get<float>(); });
        }

        [[nodiscard]] Interstellar::Data::ElementData toData() const {
            using namespace Interstellar::Data;
            return {
                Symbol,
                Name,
                AtomicNumber,
                AtomicMass,
                Density,
                MeltingPoint,
                BoilingPoint,
                AtomicRadius,
                Electronegativity,
                parseState(StateAtSTP),
                Abundance,
                TriplePointTemp,
                TriplePointPressure,
                CriticalPointTemp,
                CriticalPointPressure,
                IsMetal,
                IsToxic,
                IsMagnetic,
                DisplayColor,
                Reactivity,
                Radiation,
                CommonCompounds
            };
        }

        std::string getNamespace() const override { return "data"; }
        std::string getFilename() const override { return "elements.json"; }
    };

}
