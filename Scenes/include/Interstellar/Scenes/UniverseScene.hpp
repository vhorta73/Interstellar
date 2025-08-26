#pragma once
#include <memory>

#include <Interstellar/Input/IKeyboard.hpp>
#include <Interstellar/Input/IMouse.hpp>
#include <Interstellar/Engine/Cameras/CameraRig3D.hpp>
#include <Interstellar/Graphics/Renderers/StarsRenderer.hpp>
#include <Interstellar/Universe/Universe3DRecipe.hpp>
#include <Interstellar/Universe/Seed.hpp>
#include <Interstellar/Graphics/IGraphics.hpp>

namespace Interstellar::Scenes {

    class UniverseScene {
    public:
        UniverseScene(Interstellar::Input::IKeyboard& kb,
            Interstellar::Input::IMouse& mouse,
            Interstellar::Universe::Seed64 masterSeed);

        // Engine supplies viewport size each frame
        void update(int viewportW, int viewportH, double dt);
        void render(Interstellar::Graphics::IGraphics& gfx,
            int viewportW, int viewportH);

    private:
        Interstellar::Input::IKeyboard& kb_;
        Interstellar::Input::IMouse& mouse_;

        Interstellar::Engine::Cameras::CameraRig3D            cam_;
        std::unique_ptr<Interstellar::Graphics::Renderers::StarsRenderer> stars_;

        Interstellar::Universe::Universe3DRecipe recipe_;
        Interstellar::Universe::Seed64           masterSeed_;
    };

} // namespace Interstellar::Scenes
