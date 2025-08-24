#pragma once
#include "Interstellar/Engine/Camera2D.hpp"
#include <glm/glm.hpp>

namespace Interstellar::Engine {

    struct AABB2 {
        glm::vec2 min;
        glm::vec2 max;
    };

    /// Compute the world-space rectangle that maps to NDC [-1,1] for the current camera.
    /// Assumes your vertex shader does roughly: clip.xy = (world.xy - center) * zoom;
    inline AABB2 VisibleAABB(const Camera2D& cam, int viewportW, int viewportH) {
        const float aspect = viewportW / static_cast<float>(viewportH);
        const float halfWorldH = 1.0f / cam.zoom;          // NDC half-height 1.0 -> world half-height scales with 1/zoom
        const float halfWorldW = halfWorldH * aspect;

        AABB2 r;
        r.min = cam.center - glm::vec2(halfWorldW, halfWorldH);
        r.max = cam.center + glm::vec2(halfWorldW, halfWorldH);
        return r;
    }

} // namespace Interstellar::Engine
