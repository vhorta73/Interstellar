#pragma once
#include <cstdint>
#include <cmath>
#include <glm/glm.hpp>
#include "Interstellar/Universe/Seed.hpp"

namespace Interstellar::Universe {

    struct SectorId {
        std::int64_t x = 0;
        std::int64_t y = 0;
        bool operator==(const SectorId& o) const { return x == o.x && y == o.y; }
    };

    inline SectorId worldToSector(const glm::vec2& p, float sectorSize) {
        return { static_cast<std::int64_t>(std::floor(p.x / sectorSize)),
                 static_cast<std::int64_t>(std::floor(p.y / sectorSize)) };
    }

    inline glm::vec2 sectorOrigin(const SectorId& s, float sectorSize) {
        return { static_cast<float>(s.x) * sectorSize, static_cast<float>(s.y) * sectorSize };
    }

    inline Seed64 sectorSeed(Seed64 masterSeed, const SectorId& s) {
        return hashCombine(masterSeed, hashInts(s.x, s.y));
    }

} // namespace Interstellar::Universe
