#include "Interstellar/Scenes/UniverseScene.hpp"

namespace Interstellar::Scenes {

    using Interstellar::Input::IKeyboard;
    using Interstellar::Input::IMouse;
    using Interstellar::Universe::Seed64;
    using Interstellar::Graphics::IGraphics;
    using Interstellar::Engine::Cameras::CameraRigParams;
    using Interstellar::Graphics::Renderers::StarsRenderer;

    UniverseScene::UniverseScene(IKeyboard& kb,
        IMouse& mouse,
        Seed64 masterSeed)
        : kb_(kb)
        , mouse_(mouse)
        , masterSeed_(masterSeed)
    {
        // ---- Camera defaults ----
        cam_.center = { 0.0f, 0.0f };
        cam_.z = 2000.0f;
        cam_.fovDeg = 60.0f;

        // Tune via params()
        auto& p = cam_.params();
        p.minZ = 5.0f;
        p.maxZ = 2.0e6f;
        p.minFov = 25.0f;
        p.maxFov = 90.0f;
        p.panBase = 0.7f;    // XY pan speed
        // p.dollySpeed (wheel) / posHL / zHL / fovHL are fine as defaults

        cam_.targetCenter = cam_.center;
        cam_.targetZ = cam_.z;
        cam_.targetFovDeg = cam_.fovDeg;

        // ---- Universe recipe (tweak to taste) ----
        recipe_.sectorSize = 256.0f;
        recipe_.starDensity = 4.0e-5f;
        recipe_.jitter = 0.35f;

        // stars_ is created lazily in render() with the live IGraphics
    }

    void UniverseScene::update(int viewportW, int viewportH, double dt)
    {
        cam_.handleInput(kb_, mouse_, viewportW, viewportH, dt);
        cam_.update(dt); // (was step) - matches CameraRig3D.hpp
    }

    void UniverseScene::render(IGraphics& gfx, int viewportW, int viewportH)
    {
        // Lazy init so we can pass the concrete IGraphics right here
        if (!stars_) {
            stars_ = std::make_unique<StarsRenderer>(gfx);
        }

        stars_->render(gfx, cam_, recipe_, masterSeed_, viewportW, viewportH);
    }

} // namespace Interstellar::Scenes
