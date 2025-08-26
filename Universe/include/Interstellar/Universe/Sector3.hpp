#pragma once
#include <cstdint>
#include <cmath>
#include <glm/glm.hpp>
#include "Interstellar/Universe/Seed.hpp"

namespace Interstellar::Universe {

    struct Sector3Id {
        std::int64_t x = 0, y = 0, z = 0;
        bool operator==(const Sector3Id& o) const { return x == o.x && y == o.y && z == o.z; }
    };

    inline Sector3Id worldToSector(const glm::vec3& p, float S) {
        return {
            static_cast<std::int64_t>(std::floor(p.x / S)),
            static_cast<std::int64_t>(std::floor(p.y / S)),
            static_cast<std::int64_t>(std::floor(p.z / S))
        };
    }

    inline glm::vec3 sectorOrigin(const Sector3Id& s, float S) {
        return { static_cast<float>(s.x) * S,
                 static_cast<float>(s.y) * S,
                 static_cast<float>(s.z) * S };
    }

    inline Seed64 sectorSeed3(Seed64 masterSeed, const Sector3Id& s) {
        // stable hash of (master, x, y, z)
        Seed64 h = hashCombine(masterSeed, mix64(static_cast<Seed64>(s.x)));
        h = hashCombine(h, mix64(static_cast<Seed64>(s.y)));
        h = hashCombine(h, mix64(static_cast<Seed64>(s.z)));
        return h;
    }

} // namespace Interstellar::Universe
