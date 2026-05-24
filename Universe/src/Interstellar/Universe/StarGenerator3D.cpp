#include "Interstellar/Universe/StarGenerator3D.hpp"
#include "Interstellar/Universe/Random.hpp"
#include "Interstellar/Universe/Galaxy.hpp"

#include <algorithm>   // std::max
#include <glm/glm.hpp> // glm::clamp

namespace Interstellar::Universe {

    static constexpr Seed64 STAR_DOMAIN = 0x73a7d5b5b3f9c0e1ull;

    inline float galaxyAcceptance(const Universe3DRecipe& recipe, const glm::vec3& p) {
        if (!recipe.useGalaxy) return 1.0f;
        const float raw = GalaxyDiskMultiplier(recipe.galaxy, p);
        const float norm = raw / (1.0f + recipe.galaxy.coreBoost);
        return glm::clamp(norm, 0.0f, 1.0f);
    }

    // Unity-build safe (unique name in this TU)
    static inline int poissonCheap3D(float lambda, Rng64& rng) {
        if (lambda <= 0.0f) return 0;
        const int   base = static_cast<int>(lambda);
        const float frac = lambda - static_cast<float>(base);
        return base + ((rng.nextFloat01() < frac) ? 1 : 0);
    }

    void GenerateStarsInSector3(Seed64 masterSeed,
        const Universe3DRecipe& recipe,
        const Sector3Id& sector,
        std::vector<Star3>& outStars,
        const AABB3* aabbOpt)
    {
        const float S = recipe.sectorSize;
        const float V = S * S * S;
        const float lambda = std::max(0.0f, recipe.starDensity) * V;

        Seed64 secSeed = hashCombine(sectorSeed3(masterSeed, sector), STAR_DOMAIN);
        Rng64 rng(secSeed);

        const int count = poissonCheap3D(lambda, rng);
        outStars.reserve(outStars.size() + static_cast<size_t>(count));

        const glm::vec3 o = sectorOrigin(sector, S);
        for (int i = 0; i < count; ++i) {
            glm::vec3 p{ rng.uniform(0.0f, S), rng.uniform(0.0f, S), rng.uniform(0.0f, S) };
            p += o;

            if (aabbOpt && !contains(*aabbOpt, p)) continue;
            if (rng.nextFloat01() > galaxyAcceptance(recipe, p)) continue;

            Seed64 sid = hashCombine(secSeed, static_cast<Seed64>(i));
            float  intensity = rng.nextFloat01();
            outStars.push_back(Star3{ p, intensity, sid });
        }
    }

    void GenerateStarsInAABB3(Seed64 masterSeed,
        const Universe3DRecipe& recipe,
        const AABB3& aabb,
        std::vector<Star3>& outStars)
    {
        const float   S = recipe.sectorSize;
        const Sector3Id smin = worldToSector(aabb.min, S);
        const Sector3Id smax = worldToSector(aabb.max, S);

        for (std::int64_t z = smin.z; z <= smax.z; ++z)
            for (std::int64_t y = smin.y; y <= smax.y; ++y)
                for (std::int64_t x = smin.x; x <= smax.x; ++x) {
                    GenerateStarsInSector3(masterSeed, recipe, Sector3Id{ x, y, z }, outStars, &aabb);
                }
    }

} // namespace Interstellar::Universe
