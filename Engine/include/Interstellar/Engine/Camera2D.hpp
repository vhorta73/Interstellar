#pragma once
#include <glm/vec2.hpp>
#include <cfloat>
#include "Interstellar/Graphics/IMaterial.hpp"

namespace Interstellar::Engine {

    struct Camera2D {
        glm::vec2 center{ 0.0f, 0.0f }; // world-space
        float zoom = 1.0f;            // >1 zooms in (your shader expects u_Zoom)

        // push to material (same uniforms you're already using)
        inline void apply(Interstellar::Graphics::IMaterial& mat) const {
            float offset[2] = { -center.x, -center.y }; // negative = pan world opposite camera
            mat.Set("u_Offset", offset, sizeof(offset));
            mat.Set("u_Zoom", &zoom, sizeof(zoom));
        }

        // floating-origin: if camera drifts far, recentre and return the delta to apply to all entities
        inline glm::vec2 recenterIfNeeded(float threshold = 100000.0f) {
            if (std::abs(center.x) > threshold || std::abs(center.y) > threshold) {
                glm::vec2 delta = center;
                center = { 0.0f, 0.0f };
                return delta; // caller should subtract delta from all entity positions
            }
            return { 0.0f, 0.0f };
        }
    };

} // namespace
