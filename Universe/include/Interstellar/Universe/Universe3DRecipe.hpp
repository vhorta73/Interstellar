#pragma once
#include <cstdint>

namespace Interstellar::Universe {

    // Tunables for 3D universe (volumetric).
    // Units: sectorSize in world units; starDensity in stars per (world unit)^3.
    struct Universe3DRecipe {
        std::uint32_t rulesVersion = 1;
        float sectorSize = 256.0f;  // cubic sector edge length
        float starDensity = 1.5e-5f; // stars per cubic world unit (tweak to taste)
        float jitter = 0.35f;   // reserved for clustering later
    };

} // namespace Interstellar::Universe
