#include "CppUnitTest.h"
#include "nlohmann/json.hpp"

#include "Interstellar/Config/JsonImpl/ElementConfig.hpp"

#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Interstellar::Config::JsonImpl;
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

#include "Interstellar/Core/Logging.hpp"
const auto s_Logger = Interstellar::Core::Logger(Interstellar::Core::LOG_GRAPHIC);


namespace ElementDataTests {

    TEST_CLASS(ElementDataTests) {
public:

    TEST_METHOD(Extended_ToData_Mapping_Is_Correct) {
        nlohmann::json input = {
            {"Symbol", "Xe"},
            {"Name", "Xenon"},
            {"AtomicNumber", 54},
            {"AtomicMass", 131.29f},
            {"Group", 18},
            {"Period", 5},
            {"Block", "p"},
            {"StandardState", "gas"},
            {"Color", {0.8f, 0.9f, 1.0f}},
            {"CosmicAbundance", {{"Value", 0.000008f}, {"Unit", "ppm"}}},
            {"ElectronConfiguration", "[Kr] 4d10 5s2 5p6"},
            {"ElectronShells", {2, 8, 18, 18, 8}},
            //{"ValenceElectrons", 8},
            // ... add more test JSON
        };

       Interstellar::Config::JsonImpl::ElementConfig config;
        config.fromJson(input);
        ElementData data = config.toData();

         //Identification
        Assert::AreEqual(std::string("Xe"), data.symbol);
        Assert::AreEqual(std::string("Xenon"), data.name);
        Assert::AreEqual(54, data.atomicNumber);
        Assert::AreEqual(131.29f, data.atomicMass);
        Assert::AreEqual(18, data.group);
        Assert::AreEqual(5, data.period);
        Assert::AreEqual(std::string("p"), data.block);
        Assert::AreEqual(std::string("gas"), data.standardState);
        Assert::AreEqual(0.8f, data.colour.r);
        Assert::AreEqual(0.9f, data.colour.g);
        Assert::AreEqual(1.0f, data.colour.b);
        Assert::AreEqual(std::string("[Kr] 4d10 5s2 5p6"), data.electronConfiguration.display);
        Assert::AreEqual(std::string("Kr"), data.electronConfiguration.nobleGas);
        //Assert::AreEqual(std::vector( "[Kr] 4d10 5s2 5p6" ), data.electronConfiguration.orbitals);

        //Assert::AreEqual(140, data.quantum.covalentRadius.value.value());
        //Assert::AreEqual(std::string("Asphyxiant"), data.environmentalProp.hazards[0]);
        //Assert::AreEqual(std::string("XeF4"), data.chemical.hybridizationExamples[0].first);
        //Assert::AreEqual(std::string("sp3d2"), data.chemical.hybridizationExamples[0].second);
        //Assert::AreEqual(std::string("diamagnetic"), data.quantum.magneticOrdering);
        //Assert::AreEqual(0.5f, data.quantum.nuclearSpin);
    }

    //TEST_METHOD(ToDataProducesValidElementData) {
        //ElementConfig config;
        //config.Symbol = "He";
        //config.Name = "Helium";
        //config.AtomicNumber = 2;
        //config.AtomicMass = 4.0026f;
        //config.Density = 0.0001786f;
        //config.StateAtSTP = "gas";
        //config.DisplayColor = { 0.85f, 0.85f, 1.0f };
        //config.Reactivity = 0.0f;
        //config.Radiation = 0.0f;

        //ElementData data = config.toData();

        //Assert::IsTrue(data.isValid());
        //Assert::AreEqual(std::string("He"), data.symbol);
        //Assert::AreEqual(2, data.atomicNumber);
        //Assert::AreEqual(4.0026f, data.atomicMass);
        //Assert::AreEqual(0.0001786f, data.density);
        //Assert::AreEqual(ElementState::Gas, data.stateAtSTP);
        //Assert::AreEqual(0.0f, data.reactivity);
        //Assert::AreEqual(0.0f, data.radiation);
    //}

    //TEST_METHOD(InvalidElementDataShouldReturnFalse) {
        //ElementData data;
        //data.symbol = "";
        //data.atomicNumber = 0;
        //data.atomicMass = 0.0f;

        //Assert::IsFalse(data.isValid());
    //}

    //TEST_METHOD(ParseStateCorrectness) {
        //Assert::AreEqual(ElementState::Solid, parseState("solid"));
        //Assert::AreEqual(ElementState::Liquid, parseState("LIQUID"));
        //Assert::AreEqual(ElementState::Gas, parseState("Gas"));
        //Assert::AreEqual(ElementState::Plasma, parseState("plasma"));
        //Assert::AreEqual(ElementState::Unknown, parseState("somethingElse"));
    //}

    //TEST_METHOD(ToStringMatchesEnumValue) {
        //Assert::AreEqual("solid", to_string(ElementState::Solid));
        //Assert::AreEqual("liquid", to_string(ElementState::Liquid));
        //Assert::AreEqual("gas", to_string(ElementState::Gas));
        //Assert::AreEqual("plasma", to_string(ElementState::Plasma));
        //Assert::AreEqual("unknown", to_string(ElementState::Unknown));
    //}

    //TEST_METHOD(DeserializeFromJsonWorksCorrectly) {
        //nlohmann::json jsonElem = {
            //{"Symbol", "Ne"},
            //{"Name", "Neon"},
            //{"AtomicNumber", 10},
            //{"AtomicMass", 20.1797f},
            //{"Density", 0.0008999f},
            //{"StateAtSTP", "gas"},
            //{"DisplayColor", {0.7f, 0.9f, 1.0f}},
            //{"Reactivity", 0.0f},
            //{"Radiation", 0.0f}
        //};

        //ElementConfig config;
        //config.fromJson(jsonElem);
        //ElementData data = config.toData();

        //Assert::IsTrue(data.isValid());
        //Assert::AreEqual(std::string("Ne"), data.symbol);
        //Assert::AreEqual(10, data.atomicNumber);
        //Assert::AreEqual(ElementState::Gas, data.stateAtSTP);
    //}

    //TEST_METHOD(ToJsonPreservesFieldOrderAndContent) {
        //ElementConfig config;
        //config.Symbol = "C";
        //config.Name = "Carbon";
        //config.AtomicNumber = 6;
        //config.AtomicMass = 12.011f;
        //config.Density = 2.267f;
        //config.StateAtSTP = "solid";
        //config.DisplayColor = { 0.25f, 0.25f, 0.25f };
        //config.Reactivity = 0.5f;
        //config.Radiation = 0.0f;

        //nlohmann::json j = config.toJson();

        //Assert::AreEqual("C", j["Symbol"].get<std::string>().c_str());
        //Assert::AreEqual("Carbon", j["Name"].get<std::string>().c_str());
        //Assert::AreEqual(6, j["AtomicNumber"].get<int>());
        //Assert::AreEqual(12.011f, j["AtomicMass"].get<float>());
        //Assert::AreEqual(2.267f, j["Density"].get<float>());
        //Assert::AreEqual("solid", j["StateAtSTP"].get<std::string>().c_str());

        //const auto& color = j["DisplayColor"];
        //Assert::AreEqual(3u, static_cast<unsigned>(color.size()));
        //Assert::AreEqual(0.25f, color[0].get<float>());
        //Assert::AreEqual(0.25f, color[1].get<float>());
        //Assert::AreEqual(0.25f, color[2].get<float>());

        //Assert::AreEqual(0.5f, j["Reactivity"].get<float>());
        //Assert::AreEqual(0.0f, j["Radiation"].get<float>());
    //}

    //TEST_METHOD(StateDetermination_SolidBelowMelting) {
        //ElementData element;
        //element.symbol = "Fe";
        //element.atomicNumber = 26;
        //element.atomicMass = 55.845f;
        //element.meltingPoint = 1811.0f;
        //element.boilingPoint = 3134.0f;

        //auto state = element.getState(300.0f, 0.1f); // Well below melting point
        //Assert::AreEqual(ElementState::Solid, state);
    //}

    //TEST_METHOD(StateDetermination_LiquidBetweenMeltingAndBoiling) {
        //ElementData element;
        //element.symbol = "Hg";
        //element.atomicNumber = 80;
        //element.atomicMass = 200.59f;
        //element.meltingPoint = 234.32f;
        //element.boilingPoint = 629.88f;

        //auto state = element.getState(300.0f);
        //Assert::AreEqual(ElementState::Liquid, state);
    //}

    //TEST_METHOD(StateDetermination_GasAboveBoiling) {
        //ElementData element;
        //element.symbol = "N";
        //element.atomicNumber = 7;
        //element.atomicMass = 14.007f;
        //element.meltingPoint = 63.15f;
        //element.boilingPoint = 77.36f;

        //auto state = element.getState(100.0f); // Above boiling
        //Assert::AreEqual(ElementState::Gas, state);
    //}

    //TEST_METHOD(StateDetermination_EdgeCase_MeltingExact) {
        //ElementData element;
        //element.symbol = "O";
        //element.atomicNumber = 8;
        //element.atomicMass = 15.999f;
        //element.meltingPoint = 54.36f;
        //element.boilingPoint = 90.20f;

        //auto state = element.getState(54.36f); // Exactly at melting point
        //Assert::AreEqual(ElementState::Liquid, state); // Assumes melting point inclusive
    //}

    //TEST_METHOD(StateDetermination_EdgeCase_BoilingExact) {
        //ElementData element;
        //element.symbol = "Ar";
        //element.atomicNumber = 18;
        //element.atomicMass = 39.948f;
        //element.meltingPoint = 83.80f;
        //element.boilingPoint = 87.30f;

        //auto state = element.getState(87.30f); // Exactly at boiling
        //std::cout << state << std::endl;
        //Assert::AreEqual(ElementState::Gas, state); // Assumes boiling point inclusive
    //}

    //TEST_METHOD(PhaseStateDetermination_Works) {
        //ElementData oxygen;
        //oxygen.symbol = "O";
        //oxygen.atomicNumber = 8;
        //oxygen.atomicMass = 15.999f;
        //oxygen.meltingPoint = 54.36f;      // K
        //oxygen.boilingPoint = 90.20f;      // K
        //oxygen.criticalPointTemp = 154.6f;
        //oxygen.criticalPointPressure = 49.8f;

        //Assert::AreEqual(ElementState::Solid, oxygen.getState(40.0f));
        //Assert::AreEqual(ElementState::Liquid, oxygen.getState(70.0f));
        //Assert::AreEqual(ElementState::Plasma, oxygen.getState(160.0f, 60.0f));
        //Assert::AreEqual(ElementState::Gas, oxygen.getState(100.0f, 1.0f));
    //}

    //TEST_METHOD(PhaseStateFallsBackToUnknownIfDataIsIncomplete) {
        //ElementData incomplete;
        //incomplete.meltingPoint = 0.0f;
        //incomplete.boilingPoint = 0.0f;
        //incomplete.criticalPointTemp = 0.0f;
        //incomplete.criticalPointPressure = 0.0f;

        //Assert::AreEqual(ElementState::Unknown, incomplete.getState(500.0f, 1.0f));
    //}

    };
}
