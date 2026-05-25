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
    static constexpr float AU_KM    = 149'597'870.7f;
    static constexpr float PLUTO_AU = 39.5f;
    static constexpr float SECTOR_KM = AU_KM * PLUTO_AU;   // ≈ 5.91e9 km per sector

    UniverseScene::UniverseScene(IKeyboard& kb,
        IMouse& mouse,
        Interstellar::Graphics::IGraphics& gfx,
        Seed64 masterSeed)
        : kb_(kb)
        , mouse_(mouse)
        , masterSeed_(masterSeed)
    {
        cam_.pos      = { 0.0, 0.0, 0.0 };
        cam_.yawDeg   = 0.0f;
        cam_.pitchDeg = 0.0f;
        cam_.fovDeg   = 60.0f;

        auto& p = cam_.params();
        p.minFov            = 25.0f;  p.maxFov = 90.0f;
        // panBase and dollyKmPerNotch are overwritten each frame by the altitude-based
        // dynamic speed system; these values only govern the very first frame.
        p.panBase           = 5.0e7f;
        p.dollyKmPerNotch   = 1.0e5f;
        p.yawSensDegPerPx   = 0.1f;
        p.pitchSensDegPerPx = 0.1f;
        p.dragMult          = 0.01f;

        cam_.targetPos    = cam_.pos;
        cam_.targetFovDeg = cam_.fovDeg;

        recipe_.sectorSize = SECTOR_KM;

        {
            const long double S  = (long double)recipe_.sectorSize;
            const long double S3 = S * S * S;
            const long double lambdaTarget = 5000.0L;
            recipe_.starDensity = static_cast<float>(lambdaTarget / S3);
        }
        recipe_.jitter    = 0.035f;
        recipe_.useGalaxy = true;

        recipe_.galaxy.center        = glm::vec3(0.0f);
        recipe_.galaxy.orientation   = glm::mat3(1.0f);
        recipe_.galaxy.radialScale   = 3.0e17f;
        recipe_.galaxy.verticalScale = 3.0e15f;
        recipe_.galaxy.coreRadius    = 3.0e16f;
        recipe_.galaxy.coreBoost     = 6.0f;

        stars_ = std::make_unique<Interstellar::Graphics::Renderers::StarsRenderer>(gfx);
        stars_->setMinQueryRadius(1.0e10f);
    }


    void UniverseScene::update(int viewportW, int viewportH, double dt)
    {
        cam_.handleInput(kb_, mouse_, viewportW, viewportH, dt);
        cam_.update(dt);

        // Pick focus star on click
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
        lmbPrev_ = lmb;

        // Dynamic speed and near plane — scale with altitude above the nearest star surface.
        // panBase = 0.5 × altitude: you halve the gap per second at full WASD (Shift = 2s).
        // nearKm  = 0.001 × altitude: near plane is 0.1% of altitude, avoiding z-fighting
        //           while allowing sub-metre approach distances.
        if (stars_) {
            auto ns = stars_->nearestStar(cam_.eyeD());
            if (ns.valid) {
                const double altKm = std::max(1e-6, ns.distKm - double(ns.radiusKm));
                cam_.params().panBase         = float(altKm * 0.5);
                cam_.params().dollyKmPerNotch = float(altKm * 0.01);
                nearKm_ = float(std::max(1e-9, altKm * 1e-3));
                cam_.setDragFocusDepth(float(altKm));
            }
        }
    }

    void UniverseScene::render(IGraphics& gfx, int viewportW, int viewportH)
    {
        stars_->render(gfx, cam_, recipe_, masterSeed_, viewportW, viewportH, nearKm_);
    }

    std::string UniverseScene::hudLine() const {
        const auto  p  = cam_.eyeD();
        const auto  pF = glm::vec3(p);
        const float S  = recipe_.sectorSize;
        const auto  sid = Interstellar::Universe::worldToSector(pF, S);
        std::ostringstream os;
        os.setf(std::ios::fixed); os.precision(1);
        os << "pos[km]=(" << p.x << "," << p.y << "," << p.z << ") "
            << "sector=(" << (long long)sid.x << "," << (long long)sid.y << "," << (long long)sid.z << ") "
            << "S=" << S << " km";
        return os.str();
    }

    std::string UniverseScene::hudTitle() const {
        const auto  p  = cam_.eyeD();
        const auto  pF = glm::vec3(p);
        const float S  = recipe_.sectorSize;
        const auto  sec = Interstellar::Universe::worldToSector(pF, S);
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
