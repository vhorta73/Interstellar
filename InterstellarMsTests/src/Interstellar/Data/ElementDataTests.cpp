#include "pch.h"
#include "CppUnitTest.h"
#include "nlohmann/json.hpp"

#include "Interstellar/Config/JsonImpl/ElementConfig.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Interstellar::Config;
using namespace Interstellar::Data;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
    template<>
    std::wstring ToString<Interstellar::Data::ElementState>(const Interstellar::Data::ElementState& state) {
        using enum Interstellar::Data::ElementState;
        switch (state) {
        case Solid:   return L"Solid";
        case Liquid:  return L"Liquid";
        case Gas:     return L"Gas";
        case Plasma:  return L"Plasma";
        default:      return L"Unknown";
        }
    }
}

namespace ElementDataTests {
    //using Interstellar::Data::ElementData;
    //using Interstellar::Data::ElementState;
    TEST_CLASS(ElementDataTests) {
public:

    TEST_METHOD(ToDataProducesValidElementData) {
        ElementConfig config;
        config.Symbol = "He";
        config.Name = "Helium";
        config.AtomicNumber = 2;
        config.AtomicMass = 4.0026f;
        config.Density = 0.0001786f;
        config.StateAtSTP = "gas";
        config.DisplayColor = { 0.85f, 0.85f, 1.0f };
        config.Reactivity = 0.0f;
        config.Radiation = 0.0f;

        ElementData data = config.toData();

        Assert::IsTrue(data.isValid());
        Assert::AreEqual(std::string("He"), data.symbol);
        Assert::AreEqual(2, data.atomicNumber);
        Assert::AreEqual(4.0026f, data.atomicMass);
        Assert::AreEqual(0.0001786f, data.density);
        Assert::AreEqual(ElementState::Gas, data.stateAtSTP);
        Assert::AreEqual(0.0f, data.reactivity);
        Assert::AreEqual(0.0f, data.radiation);
    }

    TEST_METHOD(InvalidElementDataShouldReturnFalse) {
        ElementData data;
        data.symbol = "";
        data.atomicNumber = 0;
        data.atomicMass = 0.0f;

        Assert::IsFalse(data.isValid());
    }

    TEST_METHOD(ParseStateCorrectness) {
        Assert::AreEqual(ElementState::Solid, parseState("solid"));
        Assert::AreEqual(ElementState::Liquid, parseState("LIQUID"));
        Assert::AreEqual(ElementState::Gas, parseState("Gas"));
        Assert::AreEqual(ElementState::Plasma, parseState("plasma"));
        Assert::AreEqual(ElementState::Unknown, parseState("somethingElse"));
    }
    
    TEST_METHOD(ToStringMatchesEnumValue) {
        Assert::AreEqual("solid", to_string(ElementState::Solid));
        Assert::AreEqual("liquid", to_string(ElementState::Liquid));
        Assert::AreEqual("gas", to_string(ElementState::Gas));
        Assert::AreEqual("plasma", to_string(ElementState::Plasma));
        Assert::AreEqual("unknown", to_string(ElementState::Unknown));
    }

    TEST_METHOD(DeserializeFromJsonWorksCorrectly) {
        nlohmann::json jsonElem = {
            {"Symbol", "Ne"},
            {"Name", "Neon"},
            {"AtomicNumber", 10},
            {"AtomicMass", 20.1797f},
            {"Density", 0.0008999f},
            {"StateAtSTP", "gas"},
            {"DisplayColor", {0.7f, 0.9f, 1.0f}},
            {"Reactivity", 0.0f},
            {"Radiation", 0.0f}
        };

        ElementConfig config;
        config.fromJson(jsonElem);
        ElementData data = config.toData();

        Assert::IsTrue(data.isValid());
        Assert::AreEqual(std::string("Ne"), data.symbol);
        Assert::AreEqual(10, data.atomicNumber);
        Assert::AreEqual(ElementState::Gas, data.stateAtSTP);
    }

    TEST_METHOD(ToJsonPreservesFieldOrderAndContent) {
        ElementConfig config;
        config.Symbol = "C";
        config.Name = "Carbon";
        config.AtomicNumber = 6;
        config.AtomicMass = 12.011f;
        config.Density = 2.267f;
        config.StateAtSTP = "solid";
        config.DisplayColor = { 0.25f, 0.25f, 0.25f };
        config.Reactivity = 0.5f;
        config.Radiation = 0.0f;

        nlohmann::json j = config.toJson();
        Assert::AreEqual("C", j["Symbol"].get<std::string>().c_str());
        Assert::AreEqual("Carbon", j["Name"].get<std::string>().c_str());
        Assert::AreEqual(6, j["AtomicNumber"].get<int>());
        Assert::AreEqual(12.011f, j["AtomicMass"].get<float>());
        Assert::AreEqual(2.267f, j["Density"].get<float>());
        Assert::AreEqual("solid", j["StateAtSTP"].get<std::string>().c_str());
        Assert::AreEqual(3u, static_cast<unsigned>(j["DisplayColor"].size()));
        Assert::AreEqual(0.5f, j["Reactivity"].get<float>());
        Assert::AreEqual(0.0f, j["Radiation"].get<float>());
    }
    };
}
