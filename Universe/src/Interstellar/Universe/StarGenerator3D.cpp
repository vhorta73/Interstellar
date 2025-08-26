#include "Interstellar/Universe/StarGenerator3D.hpp"
#include "Interstellar/Universe/Random.hpp"

namespace Interstellar::Universe {

    // Domain separation constants so adding new layers won't perturb stars.
    static constexpr Seed64 STAR_DOMAIN = 0x73a7d5b5b3f9c0e1ull;

    void GenerateStarsInSector3(Seed64 masterSeed,
        const Universe3DRecipe& recipe,
        const Sector3Id& sector,
        std::vector<Star3>& outStars,
        const AABB3* aabbOpt)
    {
        const float S = recipe.sectorSize;
        const float V = S * S * S;
        const float lambda = std::max(0.0f, recipe.starDensity) * V;

        // Per-sector RNG seeded with domain salt
        Seed64 secSeed = hashCombine(sectorSeed3(masterSeed, sector), STAR_DOMAIN);
        Rng64 rng(secSeed);

        const int count = poissonCheap(lambda, rng);
        outStars.reserve(outStars.size() + static_cast<size_t>(count));

        const glm::vec3 o = sectorOrigin(sector, S);
        for (int i = 0; i < count; ++i) {
            // Uniform within the sector cube
            glm::vec3 p{
                rng.uniform(0.0f, S),
                rng.uniform(0.0f, S),
                rng.uniform(0.0f, S)
            };
            p += o;

            if (aabbOpt && !contains(*aabbOpt, p)) continue;

            // Stable per-star id (sector salt + local index)
            Seed64 sid = hashCombine(secSeed, static_cast<Seed64>(i));

            // Lightweight "magnitude" seed for later shading/LOD
            float intensity = rng.nextFloat01();

            outStars.push_back(Star3{ p, intensity, sid });
        }
    }

    void GenerateStarsInAABB3(Seed64 masterSeed,
        const Universe3DRecipe& recipe,
        const AABB3& aabb,
        std::vector<Star3>& outStars)
    {
        const float S = recipe.sectorSize;

        const Sector3Id smin = worldToSector(aabb.min, S);
        const Sector3Id smax = worldToSector(aabb.max, S);

        for (std::int64_t z = smin.z; z <= smax.z; ++z) {
            for (std::int64_t y = smin.y; y <= smax.y; ++y) {
                for (std::int64_t x = smin.x; x <= smax.x; ++x) {
                    GenerateStarsInSector3(masterSeed, recipe, Sector3Id{ x,y,z }, outStars, &aabb);
                }
            }
        }
    }

} // namespace Interstellar::Universe
