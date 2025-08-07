#include <iostream>
#include <sstream>
#include <algorithm>

#include "Interstellar/Interstellar.hpp"
#include "Interstellar/Graphics/Core/ITexture.hpp"
#include "Interstellar/Core/Logging.hpp"
#include "Interstellar/Graphics/OpenGL/OpenGLGraphics.hpp"
#include "Interstellar/Input/IInputManager.hpp"
#include "Interstellar/Input/IKeyboardManager.hpp"
#include "Interstellar/Input/IMouseManager.hpp"
#include "Interstellar/Graphics/Core/IShader.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace {
    constexpr auto LOG_CATEGORY = Interstellar::Core::LOG_INIT;
    constexpr const char* TEXTURE_PATH = "assets/textures/texture_01.png";

    // Vertex layout: position (x, y, z), UV (u, v)
    constexpr float vertices[] = {
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
        0.0f,  0.5f, 0.0f,   0.5f, 1.0f,
    };

    unsigned int indices[] = { 0, 1, 2 };
}

#include "Interstellar/Config/JsonImpl/ElementConfig.hpp"
#include "Interstellar/Config/JsonImpl/ElementDatabase.hpp"
#include <string>
#include "Interstellar/Data/ElementData.hpp"
int main() {
    glm::vec2 triangleOffset = glm::vec2(0.0f);
    glm::vec2 dragStart = glm::vec2(0.0f);
    bool dragging = false;
    float zoom = 1.0f; // Default zoom level

    const auto s_Logger = Interstellar::Core::Logger(LOG_CATEGORY);

    s_Logger.LogInfo("Interstellar Engine Initialisation...");
    Interstellar::Config::JsonImpl::ElementDatabase elementDb;
    if (!elementDb.loadFromFile("assets/data/elements_all.json")) {
        s_Logger.LogError("Failed to load element database from 'assets/data/elements_all.json'.");
    }
    else {
        s_Logger.LogInfo("Success loading elements from 'assets/data/elements_all.json'.");
        auto all = elementDb.all();
        for (const auto& [symbol, e] : all) {
            if (e.atomicNumber == 3) {
                s_Logger.LogInfo("Symbol - {}", e.symbol);
                s_Logger.LogInfo("Name - {}", e.name);
                s_Logger.LogInfo("AtomicNumber - {}", e.atomicNumber);
                s_Logger.LogInfo("AtomicMass - {}", e.atomicMass);
                s_Logger.LogInfo("Group - {}", e.group);
                s_Logger.LogInfo("Period - {}", e.period);
                s_Logger.LogInfo("Block - {}", e.block);
                s_Logger.LogInfo("StandardState - {}", e.standardState);
                s_Logger.LogInfo("Colour - ({}, {}, {})", e.colour.x, e.colour.y, e.colour.z);
                s_Logger.LogInfo("Cosmic Abundance - ({}, {})", e.cosmicAbundance.value, e.cosmicAbundance.unit);
                s_Logger.LogInfo("Electron Configuration - {}, {}", e.electronConfiguration.display, e.electronConfiguration.nobleGas);
                for (const auto orb : e.electronConfiguration.orbitals) {
                    s_Logger.LogInfo("   Electrons: {}, Orbital {}", orb.electrons, orb.orbital);
                }
                for (const auto shells: e.electronShells ) {
                    s_Logger.LogInfo("  Electron Shells: {}", shells);
                }
                s_Logger.LogInfo("Valence Electrons - {}", e.valenceElectrons);
                for (const auto isotope : e.isotopes) {
                    s_Logger.LogInfo("  Isotope: {}, {}, {}, {}, {}, {}",
                        isotope.name,
                        isotope.massNumber,
                        isotope.isStable ? "true" : "false",
                        isotope.naturalAbundance,
                        isotope.halfLife.has_value() ? std::to_string(isotope.halfLife.value()) : "N/A",
                        isotope.radiationType.has_value() ? isotope.radiationType.value() : "N/A"
                   );
                }

                auto density = e.physicalProperties.density;
                s_Logger.LogInfo("PhysicalProperties: Density {}, {} ",
                    density.value.has_value() ? std::to_string(density.value.value()) : "null",
                    density.unit
                );
                auto meltingpoint = e.physicalProperties.meltingPoint;
                s_Logger.LogInfo("PhysicalProperties: MeltingPoint {}, {} ",
                    meltingpoint.value.has_value() ? std::to_string(meltingpoint.value.value()) : "null",
                    meltingpoint.unit
                );
                auto boilingpoint = e.physicalProperties.boilingPoint;
                s_Logger.LogInfo("PhysicalProperties: BoilingPoint {}, {} ",
                    boilingpoint.value.has_value() ? std::to_string(boilingpoint.value.value()) : "null",
                    boilingpoint.unit
                );
                auto tripePoint = e.physicalProperties.triplePoint;
                s_Logger.LogInfo("PhysicalProperties: Triple Point Temperature {}, {} ",
                    tripePoint.temperature.value.has_value() ? std::to_string(tripePoint.temperature.value.value()) : "null",
                    tripePoint.temperature.unit
                );
                s_Logger.LogInfo("PhysicalProperties: Triple Point Pressure {}, {} ",
                    tripePoint.pressure.value.has_value() ? std::to_string(tripePoint.pressure.value.value()) : "null",
                    tripePoint.pressure.unit
                );
                auto heatcapacity = e.physicalProperties.heatCapacity;
                s_Logger.LogInfo("PhysicalProperties: HeatCapacity {}, {} ",
                    heatcapacity.value.has_value() ? std::to_string(heatcapacity.value.value()) : "null",
                    heatcapacity.unit
                );
                auto thermalConductivity = e.physicalProperties.thermalConductivity;
                s_Logger.LogInfo("PhysicalProperties: ThermalConductivity {}, {} ",
                    thermalConductivity.value.has_value() ? std::to_string(thermalConductivity.value.value()) : "null",
                    thermalConductivity.unit
                );
                auto enthalpyOgFusion = e.physicalProperties.enthalpyOfFusion;
                s_Logger.LogInfo("PhysicalProperties: enthalpyOfFusion {}, {} ",
                    enthalpyOgFusion.value.has_value() ? std::to_string(enthalpyOgFusion.value.value()) : "null",
                    enthalpyOgFusion.unit
                );
                auto enthalpyOfVaporization = e.physicalProperties.enthalpyOfVaporization;
                s_Logger.LogInfo("PhysicalProperties: enthalpyOfVaporization {}, {} ",
                    enthalpyOfVaporization.value.has_value() ? std::to_string(enthalpyOfVaporization.value.value()) : "null",
                    enthalpyOfVaporization.unit
                );
                auto entropy = e.physicalProperties.entropy;
                s_Logger.LogInfo("PhysicalProperties: ThermalConductivity {}, {} ",
                    entropy.value.has_value() ? std::to_string(entropy.value.value()) : "null",
                    entropy.unit
                );

                for (auto stateRanges : e.stateRanges) {
                    s_Logger.LogInfo("StateRanges: State {} - Tm {}/{} TM {}/{} Pm {}/{} PM {}/{} ",
                        stateRanges.state,
                        stateRanges.temperature.min.value,
                        stateRanges.temperature.min.unit,
                        stateRanges.temperature.max.value,
                        stateRanges.temperature.max.unit,
                        stateRanges.pressure.min.value,
                        stateRanges.pressure.min.unit,
                        stateRanges.pressure.max.value,
                        stateRanges.pressure.max.unit
                    );
                }

                s_Logger.LogInfo("ChemicalProperties: Electronegativity - {}/{}",
                    e.chemical.electronegativity.value.has_value() ? std::to_string(e.chemical.electronegativity.value.value()) : "null",
                    e.chemical.electronegativity.unit
                );

                s_Logger.LogInfo("ChemicalProperties: IonizationEnergy - {}/{}",
                    e.chemical.ionizationEnergy.first.value.has_value() ? std::to_string(e.chemical.ionizationEnergy.first.value.value()) : "null",
                    e.chemical.ionizationEnergy.first.unit
                );

                s_Logger.LogInfo("ChemicalProperties: ElectroAfinity - {}/{}",
                    e.chemical.electronAffinity.value.has_value() ? std::to_string(e.chemical.electronAffinity.value.value()) : "null",
                    e.chemical.electronAffinity.unit
                );

                s_Logger.LogInfo("ChemicalProperties: Reactivity - {}/{}",
                    e.chemical.reactivity.value,
                    e.chemical.reactivity.scale
                );

                for (auto oxy: e.chemical.oxidationStates) {
                    s_Logger.LogInfo("ChemicalProperties: OxydationState - {} common: {}", oxy.state, oxy.common ? "Y" : "N");
                }
                for (auto bond : e.chemical.bonding) {
                    s_Logger.LogInfo("ChemicalProperties: Bonding - {}", bond);
                }
                for (auto cc : e.chemical.bonding) {
                    s_Logger.LogInfo("ChemicalProperties: CommonCompounds- {}", cc);
                }
                for (auto hyb : e.chemical.hybridizationExamples) {
                    s_Logger.LogInfo("ChemicalProperties: HybridisationExamples - {} {} ",hyb.first, hyb.second);
                }

                auto quantum = e.quantum;
                s_Logger.LogInfo("Quantum - AtomicRadius {} / {} ",
                    quantum.atomicRadius.value.has_value() ? std::to_string(quantum.atomicRadius.value.value()) : "null",
                    quantum.atomicRadius.unit.has_value() ? quantum.atomicRadius.unit.value() : "null"
                );
                s_Logger.LogInfo("Quantum - covalentRadius {} / {} ",
                    quantum.covalentRadius.value.has_value() ? std::to_string(quantum.covalentRadius.value.value()) : "null",
                    quantum.covalentRadius.unit.has_value() ? quantum.covalentRadius.unit.value() : "null"
                );
                s_Logger.LogInfo("Quantum - vanDerWaalsRadius {} / {} ",
                    quantum.vanDerWaalsRadius.value.has_value() ? std::to_string(quantum.vanDerWaalsRadius.value.value()) : "null",
                    quantum.vanDerWaalsRadius.unit.has_value() ? quantum.vanDerWaalsRadius.unit.value() : "null"
                );
                for(auto spin : quantum.spinStates )
                    s_Logger.LogInfo("Quantum - SpinStates {} ", std::to_string(spin));

                s_Logger.LogInfo("Quantum - magneticOrdering {} ", quantum.magneticOrdering);
                s_Logger.LogInfo("Quantum - isParamagnetic {} ", quantum.isParamagnetic ? "True" : "False");
                s_Logger.LogInfo("Quantum - nuclearSpin {} ", std::to_string(quantum.nuclearSpin));

                auto envProp = e.environmentalProp;
                s_Logger.LogInfo("Environmental Properties - Solubility In Water : {} / {} ",
                    envProp.solubilityInWater.value.has_value() ? std::to_string(envProp.solubilityInWater.value.value()) : "null",
                    envProp.solubilityInWater.unit.value()
                );
                s_Logger.LogInfo("Environmental Properties - Tendency In Water : Tendency: {} Spontaneous: {} RequiresSpark: {} ",
                    envProp.oxidationInAir.tendency,
                    envProp.oxidationInAir.spontaneous ? "True" : "False",
                    envProp.oxidationInAir.requiresSpark ? "True" : "False"
                );
                s_Logger.LogInfo("Environmental Properties - Toxidity {} ", envProp.toxicity);
                s_Logger.LogInfo("Environmental Properties - Biological Role {} ", envProp.biologicalRole);
                for(const auto h : envProp.hazards)
                    s_Logger.LogInfo("Environmental Properties - Hazards {} ", h);

                //s_Logger.LogInfo("{}", e.group);
            }
        }
    }


    //Interstellar::Core::Logger s_Logger(LOG_CATEGORY);
    // ==== TEMPORARY TEST CODE ====
    constexpr int windowWidth = 1200;
    constexpr int windowHeight = 860;

    auto graphics = std::make_unique<Interstellar::Graphics::OpenGL::OpenGLGraphics>();
    if (!graphics->Initialise(windowWidth, windowHeight, false)) {
        std::cerr << "[Error] Failed to initialise OpenGL graphics backend.\n";
        return -1;
    }

    auto window = static_cast<GLFWwindow*>(graphics->GetNativeWindow());
    auto input = Interstellar::Input::IInputManager::Create(window);
    auto& keyboard = input->GetKeyboardManager();
    auto& mouse = input->GetMouseManager();

    auto shader = graphics->CreateShader("TriangleShader");
    auto mesh = graphics->CreateMesh(vertices, sizeof(vertices), indices, sizeof(indices));
    auto pipeline = graphics->CreatePipeline(shader);
    auto texture = graphics->CreateTexture(TEXTURE_PATH);

    if (!shader || !mesh || !pipeline || !texture) {
        if (!shader)   s_Logger.LogError("Shader failed to compile or load.");
        if (!mesh)     s_Logger.LogError("Mesh failed to initialize.");
        if (!pipeline) s_Logger.LogError("Pipeline creation failed.");
        if (!texture)  s_Logger.LogError("Texture loading failed.");
        return -2;
    }

    // Temporary FPS tracking
    double lastTime = glfwGetTime();
    double fpsAvg = 0.0;
    static double timeAccumulator = 0.0;

    while (!graphics->ShouldClose()) {
        // Frame timing
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        double fps = 1.0 / deltaTime;
        fpsAvg = ( fps + ( fpsAvg * 9.0 ) ) / 10.0;
        timeAccumulator += deltaTime;

        if (timeAccumulator >= 1.0) {
          s_Logger.LogDebug("FPS: {} | Avg: {} ",fps, fpsAvg);
          timeAccumulator = 0.0;
        }
        input->Update(); // Refresh input states

        float scrollY = static_cast<float>(mouse.GetScrollOffsetY());
        if (scrollY != 0.0f) {

            float zoomDelta = scrollY * 0.5f;
            zoom = zoomDelta;
            zoom = std::clamp(zoom, 0.1f, 5000.0f);
        }

        // Keyboard movement
        float moveSpeed = 0.5f * static_cast<float>(deltaTime);
        if (keyboard.IsKeyDown(GLFW_KEY_LEFT))  triangleOffset.x -= moveSpeed;
        if (keyboard.IsKeyDown(GLFW_KEY_RIGHT)) triangleOffset.x += moveSpeed;
        if (keyboard.IsKeyDown(GLFW_KEY_UP))    triangleOffset.y += moveSpeed;
        if (keyboard.IsKeyDown(GLFW_KEY_DOWN))  triangleOffset.y -= moveSpeed;

        // Mouse drag
        if (mouse.IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            glm::vec2 current = glm::vec2(mouse.GetX(), mouse.GetY());

            if (!dragging) {
                dragging = true;
                dragStart = current;
            }

            if (mouse.IsDragging()) {
                glm::vec2 delta = (current - dragStart) / glm::vec2(windowWidth, windowHeight);
                delta.y *= -1.0f; // Invert Y for OpenGL
                triangleOffset += delta * 2.0f;
                dragStart = current;
            }
        }
        else {
            dragging = false;
        }
        // Render
        graphics->BeginFrame();

        // Manually bind texture to unit 0
        glActiveTexture(GL_TEXTURE0);
        if (texture) {
            auto native = texture->GetNativeHandle();
            if (native) {
                glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<uintptr_t>(native)));
            }
            else {
                s_Logger.LogWarn("Texture '{}' has no valid native handle!");// , texture->GetName());
            }
        }
        else {
            s_Logger.LogWarn("Texture was not created (nullptr).");
        }

        // Send u_Offset to the shader
        //auto glShader = std::static_pointer_cast<Interstellar::Graphics::OpenGL::OpenGLShader>(shader);
        GLuint programID = static_cast<GLuint>(reinterpret_cast<uintptr_t>(shader->GetNativeHandle()));
        glUseProgram(programID);
        GLint offsetLoc = glGetUniformLocation(programID, "u_Offset");
        if (offsetLoc >= 0) {
            glUniform2f(offsetLoc, triangleOffset.x, triangleOffset.y);
        }
        GLint zoomLoc = glGetUniformLocation(programID, "u_Zoom");
        if (zoomLoc >= 0) {
            glUniform1f(zoomLoc, zoom);
        }


        graphics->SubmitMesh(mesh, pipeline);
        graphics->EndFrame();
    }

    graphics->Shutdown();
    glfwTerminate();
    // ==== END OF TEMPORARY TEST CODE ====

    // ==== GAME INIT (NOT YET REACHED) ====
    const auto genericLogger = Interstellar::Core::Logger();

    Interstellar::Game game;

    try {
        game.loadConfig();
        game.buildComponents();
        game.start();
        game.shutdown();
    }
    catch (const std::exception& e) {
        genericLogger.LogCritical(std::string("Game error: ") + e.what());
        return 99;
    }

    s_Logger.LogInfo(std::string("Game closed with success."));

    return 0;
}