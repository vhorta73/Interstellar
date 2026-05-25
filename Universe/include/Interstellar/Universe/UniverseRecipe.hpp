#pragma once
#include <cstdint>

namespace Interstellar::Universe {

    // Tunables for the whole universe generation pass.
    // Units: sectorSize in world units; starDensity in stars per (world unit)^2.
    struct UniverseRecipe {
        std::uint32_t rulesVersion = 1;
        float sectorSize = 96.0f;   // tile world into squares of this size
        float starDensity = 0.08f;   // ~stars per square world unit (keep small)
        float jitter = 0.45f;   // currently used as random placement factor [0..1]
    };

} // namespace Interstellar::Universe
