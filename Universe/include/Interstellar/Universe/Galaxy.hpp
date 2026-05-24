#pragma once
#include <glm/glm.hpp>
#include <cmath>        // std::exp, std::sqrt, std::abs
#include <algorithm>    // std::max

namespace Interstellar::Universe {

    // Simple axis-aligned "galaxy disk" model in its own local frame.
    // Identity orientation => disk lies on the XY plane, Z is thickness.
    struct GalaxyDisk {
        glm::vec3 center{ 0.0f, 0.0f, 0.0f };

        // Columns are basis vectors; identity = world axes
        glm::mat3 orientation = glm::mat3(1.0f);

        // Scales (world units)
        float radialScale = 250000.0f; // "radius" falloff (sigma) in the disk plane
        float verticalScale = 2000.0f; // thickness (exponential) along disk normal

        // Central bulge
        float coreRadius = 15000.0f; // bulge radius (sigma)
        float coreBoost = 8.0f; // extra multiplier at the core
    };

    // Density multiplier m(x) ~ [0, 1+coreBoost]; higher near the disk mid-plane and center.
    inline float GalaxyDiskMultiplier(const GalaxyDisk& g, const glm::vec3& worldPos)
    {
        // Transform into galaxy local frame
        const glm::vec3 q = glm::transpose(g.orientation) * (worldPos - g.center);
        const float r2 = q.x * q.x + q.y * q.y;       // radial squared in disk plane
        const float h = std::abs(q.z);               // height above/below disk

        // Radial Gaussian in plane, exponential in thickness
        const float radial = std::exp(-r2 / (2.0f * g.radialScale * g.radialScale));
        const float vertical = std::exp(-h / std::max(1e-3f, g.verticalScale));
        const float core = std::exp(-r2 / (2.0f * g.coreRadius * g.coreRadius));

        // Base disk + boosted core
        return radial * vertical * (1.0f + g.coreBoost * core);
    }

} // namespace Interstellar::Universe
