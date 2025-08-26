#pragma once
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Interstellar/Graphics/IGraphics.hpp"
#include "Interstellar/Graphics/IShader.hpp"
#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IMaterial.hpp"

#include "Interstellar/Engine/Cameras/CameraRig3D.hpp"
#include "Interstellar/Universe/Universe3DRecipe.hpp"
#include "Interstellar/Universe/UniverseQuery.hpp"
#include "Interstellar/Universe/Seed.hpp"

namespace Interstellar::Graphics::Renderers {

    /**
     * Minimal star field renderer (backend-agnostic interface).
     *  - Builds a deterministic star list for the current view slab.
     *  - Submits as GL_POINTS via OpenGL when available.
     */
    class StarsRenderer {
    public:
        // Create GPU resources (shader/pipeline/material) up front.
        explicit StarsRenderer(IGraphics& gfx);

        // Draw current star field for the given camera and viewport.
        void render(IGraphics& gfx,
            const Engine::Cameras::CameraRig3D& cam,
            const Universe::Universe3DRecipe& recipe,
            Universe::Seed64 masterSeed,
            int viewportW, int viewportH);

    private:
        // GPU resources
        std::shared_ptr<IShader>         shader_;
        std::shared_ptr<IRenderPipeline> pipeline_;
        std::shared_ptr<IMaterial>       material_;

        // Cached CPU buffers
        std::vector<float> starXYZ_; // packed x,y,z floats

        // Visual params
        glm::vec3 tint_ = glm::vec3(1.0f);
        float     minBrightness_ = 0.00006f;
        float     maxBrightness_ = 2.25f;
        float     minPx_ = 0.01f;
        float     maxPx_ = 9.0e6f;
        float     brightNear_ = 0.01f;
        float     brightFar_ = 4.0e6f;
        float     baseSizeAtUnit_ = 1.0f; // pixels at unit depth (scaled by projection)
    };

} // namespace Interstellar::Graphics::Renderers
