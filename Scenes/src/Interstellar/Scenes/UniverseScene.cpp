#include "Interstellar/Scenes/UniverseScene.hpp"
#include <algorithm>
#include <limits>
#include <sstream>
#include <glm/glm.hpp>

namespace Interstellar::Scenes {

    using Interstellar::Input::IKeyboard;
    using Interstellar::Input::IMouse;
    using Interstellar::Input::MouseButton;
    using Interstellar::Universe::Seed64;
    using Interstellar::Graphics::IGraphics;
    using Interstellar::Engine::Cameras::CameraRig3D;

    // Astronomical scale
    static constexpr float AU_KM = 149'597'870.7f;
    static constexpr float PLUTO_AU = 39.5f;                     // mean orbital radius
    static constexpr float SECTOR_KM = AU_KM * PLUTO_AU;          // ≈ 5.91e9 km per sector

    UniverseScene::UniverseScene(IKeyboard& kb,
        IMouse& mouse,
        Seed64 masterSeed)
        : kb_(kb)
        , mouse_(mouse)
        , masterSeed_(masterSeed)
    {
        // Camera
        cam_.pos = { 0.0f, 0.0f, 0.0f };
        cam_.yawDeg = 0.0f;
        cam_.pitchDeg = 0.0f;
        cam_.fovDeg = 60.0f;

        auto& p = cam_.params();
        p.minFov = 25.0f;  p.maxFov = 90.0f;
        p.panBase = 5.0e7f;            // km/s for WASD
        p.dollyKmPerNotch = 100.0e9f;    // wheel base step (scales with focus)
        p.yawSensDegPerPx = 0.12f;
        p.pitchSensDegPerPx = 0.12f;
        p.dragMult = 1.0f;

        cam_.targetPos = cam_.pos;
        cam_.targetFovDeg = cam_.fovDeg;

        // Universe recipe
        recipe_.sectorSize = SECTOR_KM;

        // Mean stars / sector
        {
            const long double S = (long double)recipe_.sectorSize;
            const long double S3 = S * S * S;
            const long double lambdaTarget = 5.0L; // ~5 per sector
            recipe_.starDensity = static_cast<float>(lambdaTarget / S3);
        }
        recipe_.jitter = 0.035f;
        recipe_.useGalaxy = true;

        // Very rough galaxy falloff (feel-based)
        recipe_.galaxy.center = glm::vec3(0.0f);
        recipe_.galaxy.orientation = glm::mat3(1.0f);
        recipe_.galaxy.radialScale = 3.0e17f;  // ~10 kpc
        recipe_.galaxy.verticalScale = 3.0e15f;  // ~300 ly
        recipe_.galaxy.coreRadius = 3.0e16f;  // ~1 kpc
        recipe_.galaxy.coreBoost = 6.0f;
    }


    void UniverseScene::update(int viewportW, int viewportH, double dt)
    {
        cam_.handleInput(kb_, mouse_, viewportW, viewportH, dt);
        cam_.update(dt);

        // Pick focus star on click (when not rotating)
        const bool lmb = mouse_.isDown(MouseButton::Left);
        if (lmb && !lmbPrev_) {
            if (stars_) {
                const auto posPx = mouse_.position();
                constexpr float kPadPx = 28.0f;
                Interstellar::Graphics::Renderers::StarsRenderer::PickResult hit{};
                if (stars_->pickNearestScreen(cam_, viewportW, viewportH, posPx.x, posPx.y, kPadPx, hit)) {
                    cam_.setFocusWorld(hit.pos);
                    cam_.setDragFocusDepth(hit.distance);
                }
                else {
                    cam_.clearFocusWorld();
                    cam_.clearDragFocusDepth();
                }
            }
        }

        // Keep 2D pan scaling coherent while dragging after a focus pick
        if (lmb) {
            if (auto f = cam_.focusWorld()) {
                cam_.setDragFocusDepth(glm::distance(cam_.eye(), *f));
            }
        }
        if (!lmb && lmbPrev_) {
            cam_.clearDragFocusDepth();
        }
        lmbPrev_ = lmb;
    }

    void UniverseScene::render(IGraphics& gfx, int viewportW, int viewportH)
    {
        if (!stars_) {
            stars_ = std::make_unique<Interstellar::Graphics::Renderers::StarsRenderer>(gfx);

            // Local bubble: big enough for Pluto-scale sectors
            stars_->setMinQueryRadius(1.0e10f); // 10 billion km

            // ENABLE world-anchored background so the sky never vanishes
            stars_->setBackground(true, 3.0e11f, 6000);
        }

        stars_->render(gfx, cam_, recipe_, masterSeed_, viewportW, viewportH);
    }

    std::string UniverseScene::hudLine() const {
        const auto  p = cam_.eye();
        const float  S = recipe_.sectorSize;
        const auto  sid = Interstellar::Universe::worldToSector(p, S);
        std::ostringstream os;
        os.setf(std::ios::fixed); os.precision(1);
        os << "pos[km]=(" << p.x << "," << p.y << "," << p.z << ") "
            << "sector=(" << (long long)sid.x << "," << (long long)sid.y << "," << (long long)sid.z << ") "
            << "S=" << S << " km";
        return os.str();
    }

    std::string UniverseScene::hudTitle() const {
        const glm::vec3 p = cam_.eye();
        const float S = recipe_.sectorSize;
        const auto sec = Interstellar::Universe::worldToSector(p, S);
        std::ostringstream os;
        os << "x:" << static_cast<long long>(std::llround(p.x))
            << "  y:" << static_cast<long long>(std::llround(p.y))
            << "  z:" << static_cast<long long>(std::llround(p.z))
            << "  quadrant:(" << static_cast<long long>(sec.x)
            << "," << static_cast<long long>(sec.y)
            << "," << static_cast<long long>(sec.z) << ")";
        return os.str();
    }

} // namespace Interstellar::Scenes
