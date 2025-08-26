#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IMaterial.hpp"
#include "Interstellar/Graphics/IShader.hpp"
#include "Interstellar/Graphics/IGraphics.hpp"
#include "Interstellar/Universe/StarGenerator3D.hpp" // Star3

namespace Interstellar::Graphics {

    struct StarVisualParams {
        glm::vec3 tint{ 1.0f, 1.0f, 1.0f };
        float     minBrightness = 0.01f;
        float     maxBrightness = 1.40f;
        float     minPx = 1.0f;
        float     maxPx = 140.0f;
        float     brightNear = 10000.0f;
        float     brightFar = 200000.0f;
        float     baseSizePxAtUnitDepth = 1.0f; // used to derive u_PointScale per frame
    };

    class StarsRenderer {
    public:
        explicit StarsRenderer(IGraphics& gfx);

        void setVisuals(const StarVisualParams& v);

        // Draw a batch of stars (3D points) using perspective point sprites.
        void draw(const std::vector<Interstellar::Universe::Star3>& stars,
            const glm::mat4& VP,
            const glm::vec3& camPos,
            int viewportHeight,
            float fovDeg);

    private:
        IGraphics* gfx_ = nullptr;
        std::shared_ptr<IRenderPipeline> pipeline_;
        std::shared_ptr<IMaterial>       material_;
        StarVisualParams vis_;

        // scratch buffer
        std::vector<float> xyz_;
    };

} // namespace Interstellar::Graphics
