#pragma once
#include <cstdint>
#include "Interstellar/Universe/Galaxy.hpp"

namespace Interstellar::Universe {

    struct Universe3DRecipe {
        // Sectoring / density
        float sectorSize = 256.0f;   // world units (cube side)
        float starDensity = 4.0e-10f;  // stars per world^3 unit
        float jitter = 0.35f;    // reserved for clustering later

        // Galaxy shaping (enable to bias density into a disk + bulge)
        bool      useGalaxy = true;
        GalaxyDisk galaxy;            // defaults are set in Galaxy.hpp ctor values
    };

} // namespace Interstellar::Universe
