#include <iostream>
#include <memory>
#include <GLFW/glfw3.h>

#include "Interstellar/Engine/Engine.hpp"
#include "Interstellar/Logging/Logging.hpp"
#include <Interstellar/Input/Input.hpp>

#include "Interstellar/Renderers/OpenGL/OpenGLGraphics.hpp"
#include "Interstellar/Scenes/UniverseScene.hpp"
#include "Interstellar/Universe/Seed.hpp"
#include "Interstellar/Universe/Universe3DRecipe.hpp"

using namespace Interstellar;
using namespace Interstellar::Input;
using namespace Interstellar::Renderers::OpenGL;
using Interstellar::Engine::Engine;
using Interstellar::Logging::LogInit;

int main() {
    constexpr int W = 960, H = 640;

    auto gfx = std::make_unique<OpenGLGraphics>();
    if (!gfx->Initialise(W, H, false)) {
        std::cerr << "Failed to init OpenGL\n";
        return 1;
    }

    GLFWwindow* window = static_cast<GLFWwindow*>(gfx->GetNativeWindow());

    // Input
    InputConfig cfg;
    cfg.nativeWindow = window;
    cfg.backend = InputConfig::Backend::GLFW;
    auto input = InputSystem::Create(cfg);
    auto& keyboard = input->keyboard();   // IKeyboard&
    auto& mouse = input->mouse();      // IMouse&

    // seed
    const Interstellar::Universe::Seed64 masterSeed =
        Interstellar::Universe::seedFromString("Interstellar-Alpha-001");
    Universe::Universe3DRecipe recipe;
    // construct the scene with *references*
    Interstellar::Scenes::UniverseScene scene(keyboard, mouse, masterSeed);

    // engine loop
    Interstellar::Engine::Engine engine;
    engine.run(
        [&](double dt) {
            // query current framebuffer size for the scene
            int w, h;
            glfwGetFramebufferSize(window, &w, &h);

            input->pump();
            input->beginFrame(dt);
            scene.update(w, h, dt);
            input->endFrame();
        },
        [&](double /*ft*/) {
            int w, h; glfwGetFramebufferSize(window, &w, &h);
            gfx->BeginFrame();
            scene.render(*gfx, w, h);
            gfx->EndFrame();
        },
        [&] { return gfx->ShouldClose(); }
    );

    gfx->Shutdown();
    glfwTerminate();
    return 0;
}
