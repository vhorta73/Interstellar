#include "Interstellar/Universe/StarGenerator.hpp"
#include "Interstellar/Universe/Random.hpp"
#include <algorithm>
#include <cmath>

namespace Interstellar::Universe {

    // Very cheap Poisson-like sampler: n = floor(lambda) (+1 with fractional prob)
    static inline int poissonCheap(float lambda, Rng64& rng) {
        if (lambda <= 0.f) return 0;
        const int base = static_cast<int>(lambda);
        const float frac = lambda - static_cast<float>(base);
        return base + ((rng.nextFloat01() < frac) ? 1 : 0);
    }

    void GenerateStarsInSector(Seed64 masterSeed,
        const UniverseRecipe& recipe,
        const SectorId& sector,
        std::vector<glm::vec2>& outStars,
        const glm::vec2* aabbMin,
        const glm::vec2* aabbMax)
    {
        const float S = recipe.sectorSize;
        const glm::vec2 origin = sectorOrigin(sector, S);

        // Sector-local RNG from masterSeed + sector id
        Rng64 rng(sectorSeed(masterSeed, sector));

        // Expected count - density x area
        const float area = S * S;
        const float lambda = std::max(0.0f, recipe.starDensity) * area;

        const int count = poissonCheap(lambda, rng);

        outStars.reserve(outStars.size() + static_cast<size_t>(count));
        for (int i = 0; i < count; ++i) {
            // Uniform placement within sector square
            float rx = rng.uniform(0.0f, S);
            float ry = rng.uniform(0.0f, S);

            // Add some "sub-grid" jitter feel if wanted (currently uniform covers that)
            // You can later cluster with recipe.jitter via Gaussian, Voronoi, etc.
            glm::vec2 p = origin + glm::vec2(rx, ry);

            if (aabbMin && aabbMax) {
                if (p.x < aabbMin->x || p.y < aabbMin->y || p.x > aabbMax->x || p.y > aabbMax->y)
                    continue;
            }
            outStars.push_back(p);
        }
    }

    void GenerateStarsInAABB(Seed64 masterSeed,
        const UniverseRecipe& recipe,
        const glm::vec2& aabbMin,
        const glm::vec2& aabbMax,
        std::vector<glm::vec2>& outStars)
    {
        const float S = recipe.sectorSize;

        // Sectors intersecting the AABB
        const auto minSec = worldToSector(aabbMin, S);
        const auto maxSec = worldToSector(aabbMax, S);

        for (std::int64_t sy = minSec.y; sy <= maxSec.y; ++sy) {
            for (std::int64_t sx = minSec.x; sx <= maxSec.x; ++sx) {
                SectorId sid{ sx, sy };
                GenerateStarsInSector(masterSeed, recipe, sid, outStars, &aabbMin, &aabbMax);
            }
        }
    }

} // namespace Interstellar::Universe
