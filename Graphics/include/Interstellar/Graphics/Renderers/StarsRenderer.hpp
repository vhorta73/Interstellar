#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Interstellar/Graphics/IGraphics.hpp"
#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IMaterial.hpp"
#include "Interstellar/Graphics/IShader.hpp"
#include "Interstellar/Engine/Cameras/CameraRig3D.hpp"
#include "Interstellar/Universe/Universe3DRecipe.hpp"
#include "Interstellar/Universe/UniverseQuery.hpp"
#include "Interstellar/Universe/Seed.hpp"
#include "Interstellar/Universe/AABB.hpp"

namespace Interstellar::Graphics::Renderers {

    class StarsRenderer {
    public:
        explicit StarsRenderer(IGraphics& gfx);

        // Near-field query radius (km)
        void setMinQueryRadius(float rKm) { queryRadiusKm_ = rKm; }

        // “Static” sky: stars beyond near query are reprojected on a camera-anchored shell
        void setBackground(bool enabled, float radiusKm, int count) {
            bgEnabled_ = enabled;
            bgRadiusKm_ = std::max(1.0f, radiusKm);
            bgCount_ = std::max(0, count);
            // legacy leftover—unused now but kept for ABI
            bgUnitDirs_.clear();
        }

        void render(IGraphics& gfx,
            const Interstellar::Engine::Cameras::CameraRig3D& cam,
            const Interstellar::Universe::Universe3DRecipe& recipe,
            Interstellar::Universe::Seed64 masterSeed,
            int viewportW, int viewportH);

        struct PickResult {
            std::size_t index{};
            glm::vec3   pos{};
            float       distance{};
            Universe::Seed64 id{};
        };

        bool pickNearestScreen(const Engine::Cameras::CameraRig3D& cam,
            int viewportW, int viewportH,
            float mouseX, float mouseY,
            float radiusPx,
            PickResult& out) const;

    private:
        // Kept for compatibility; no longer used.
        void ensureBackgroundDirs(Interstellar::Universe::Seed64) {}

        std::shared_ptr<IShader>         shader_;
        std::shared_ptr<IRenderPipeline> pipeline_;
        std::shared_ptr<IMaterial>       material_;

        // Visual tuning
        glm::vec3 tint_ = glm::vec3(1.0f);
        float     minBrightness_ = 0.0030f;     // lift floor a bit so they never “disappear”
        float     maxBrightness_ = 2.25f;
        float     minPx_ = 1.0f;                // at least 1px
        float     maxPx_ = 96.0f;               // keep giant discs in check

        // Distance→brightness window (does NOT depend on FOV)
        float     brightNear_ = 5.0e9f;
        float     brightFar_ = 5.0e12f;
        float     baseSizeAtUnit_ = 1.0f;

        // Near-field query radius (km)
        float queryRadiusKm_ = 1.0e10f;
        static constexpr float kMaxQueryKm = 1.0e12f;

        // Background impostor shell (camera-anchored)
        bool  bgEnabled_ = true;
        float bgRadiusKm_ = 3.0e11f;            // visual shell radius
        int   bgCount_ = 6000;                  // max number of backdrop points
        std::vector<glm::vec3> bgUnitDirs_;     // legacy, unused

        // Temp buffers
        std::vector<float>            starXYZ_;
        std::vector<glm::vec3>        starsWorld_;
        std::vector<Universe::Star3>  lastStars_;
        std::vector<glm::vec3>        farShellDirs_; // directions of far stars → shell
    };

} // namespace Interstellar::Graphics::Renderers
