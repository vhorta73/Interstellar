#pragma once
#include <memory>

#include <Interstellar/Input/IKeyboard.hpp>
#include <Interstellar/Input/IMouse.hpp>
#include <Interstellar/Engine/Cameras/CameraRig3D.hpp>
#include <Interstellar/Graphics/Renderers/StarsRenderer.hpp>
#include <Interstellar/Universe/Universe3DRecipe.hpp>
#include <Interstellar/Universe/Seed.hpp>
#include <Interstellar/Graphics/IGraphics.hpp>
#include <Interstellar/Universe/Sector3.hpp>  // worldToSector(..), Sector3Id

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

        // Build a one-line HUD string with pos + sector
        std::string hudLine() const;
        std::string hudTitle() const;
    private:
        Interstellar::Input::IKeyboard& kb_;
        Interstellar::Input::IMouse& mouse_;

        Interstellar::Engine::Cameras::CameraRig3D                            cam_;
        std::unique_ptr<Interstellar::Graphics::Renderers::StarsRenderer>     stars_;

        Interstellar::Universe::Universe3DRecipe recipe_;
        Interstellar::Universe::Seed64           masterSeed_;

        bool lmbPrev_ = false; // left mouse button previous state
        double hudLogAccum_ = 0.0;   // throttle console HUD logs
    };

} // namespace Interstellar::Scenes
