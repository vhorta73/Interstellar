#pragma once
#include <string>
#include <vector>
#include "Interstellar/Universe/Universe3DRecipe.hpp"
#include "Interstellar/Universe/Defaults3D.hpp"

namespace Interstellar::Universe {

    struct ValidationWarnings {
        std::vector<std::string> messages;
        void warn(const std::string& s) { messages.push_back(s); }
        bool empty() const { return messages.empty(); }
    };

    // Fills missing/invalid fields with defaults and records warnings.
    inline void ValidateAndFillDefaults(Universe3DRecipe& r, ValidationWarnings& out)
    {
        using D = Defaults3D;

        if (!(r.sectorSize > 0.0f)) {
            r.sectorSize = D::SectorSize;
            out.warn("Universe3DRecipe.sectorSize not set (<=0). Using default 256.");
        }
        if (!(r.starDensity >= 0.0f)) {
            r.starDensity = D::StarDensity;
            out.warn("Universe3DRecipe.starDensity not set (<0). Using default 4.0e-19.");
        }
        if (!(r.jitter >= 0.0f && r.jitter <= 1.0f)) {
            r.jitter = D::Jitter;
            out.warn("Universe3DRecipe.jitter out of [0,1]. Using default 0.35.");
        }

        // Galaxy block
        if (!r.useGalaxy) return;

        bool patched = false;
        if (!(r.galaxy.radialScale > 0.0f)) { r.galaxy.radialScale = D::GalRadial;   patched = true; }
        if (!(r.galaxy.verticalScale > 0.0f)) { r.galaxy.verticalScale = D::GalVertical; patched = true; }
        if (!(r.galaxy.coreRadius > 0.0f)) { r.galaxy.coreRadius = D::GalCoreRadius; patched = true; }
        if (!(r.galaxy.coreBoost >= 0.0f)) { r.galaxy.coreBoost = D::GalCoreBoost; patched = true; }

        if (patched) {
            out.warn("Universe3DRecipe.galaxy had invalid fields. Missing values were defaulted.");
        }
    }

} // namespace Interstellar::Universe
