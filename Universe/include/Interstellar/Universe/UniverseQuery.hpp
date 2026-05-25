#pragma once
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

#include "Interstellar/Universe/Seed.hpp"
#include "Interstellar/Universe/AABB.hpp"
#include "Interstellar/Universe/Universe3DRecipe.hpp"
#include "Interstellar/Universe/StarGenerator3D.hpp" // for Star3
#include "Interstellar/Universe/Random.hpp"          // Rng64
#include "Interstellar/Universe/GridIndex.hpp"       // floor_div_to_i64, sector_seed

namespace Interstellar::Universe {

    struct UniverseQueryFilters {
        bool stars = true;
        // Future: bool galaxies = false; bool nebulae = false; etc.
    };

    struct UniverseQueryResult {
        std::vector<Star3> stars;
        void clear() { stars.clear(); }
    };

    // Poisson draw from our RNG; stable per-sector
    inline int poisson_from_rng(double lambda, Rng64& rng) {
        if (lambda <= 0.0) return 0;
        if (lambda < 30.0) {
            // Knuth exact
            const double L = std::exp(-lambda);
            int k = 0;
            double p = 1.0;
            do { ++k; p *= rng.nextFloat01(); } while (p > L);
            return k - 1;
        }
        else {
            // Normal approximation via Box-Muller
            float u1 = std::max(1e-12f, rng.nextFloat01());
            float u2 = std::max(1e-12f, rng.nextFloat01());
            const double r = std::sqrt(-2.0 * std::log(u1));
            const double n = r * std::cos(2.0 * 3.141592653589793 * u2);
            const double x = lambda + std::sqrt(lambda) * n;
            return static_cast<int>(std::floor(std::max(0.0, x) + 0.5));
        }
    }

    inline void QueryUniverseAABB3(Seed64 masterSeed,
        const Universe3DRecipe& recipe,
        const AABB3& aabb,
        const UniverseQueryFilters& filters,
        UniverseQueryResult& out)
    {
        if (!filters.stars) return;

        const double S = static_cast<double>(recipe.sectorSize);
        if (S <= 0.0) return;

        const double volSector = S * S * S;
        const double lambdaPerSector = static_cast<double>(recipe.starDensity) * volSector;

        // Robust integer sector indices (works for negatives too)
        const int64_t ix0 = floor_div_to_i64(aabb.min.x, S);
        const int64_t iy0 = floor_div_to_i64(aabb.min.y, S);
        const int64_t iz0 = floor_div_to_i64(aabb.min.z, S);
        const int64_t ix1 = floor_div_to_i64(aabb.max.x, S);
        const int64_t iy1 = floor_div_to_i64(aabb.max.y, S);
        const int64_t iz1 = floor_div_to_i64(aabb.max.z, S);

        for (int64_t iz = iz0; iz <= iz1; ++iz) {
            for (int64_t iy = iy0; iy <= iy1; ++iy) {
                for (int64_t ix = ix0; ix <= ix1; ++ix) {

                    // Per-sector RNG seed (ONLY depends on master + (ix,iy,iz))
                    Rng64 rng(sector_seed(masterSeed, ix, iy, iz, /*salt*/0xA7F3B1D5ULL));

                    // Stable per-sector star count
                    const int count = poisson_from_rng(lambdaPerSector, rng);

                    // Sector origin in world space
                    const double sx = static_cast<double>(ix) * S;
                    const double sy = static_cast<double>(iy) * S;
                    const double sz = static_cast<double>(iz) * S;

                    for (int n = 0; n < count; ++n) {
                        // Uniform position inside the sector cube
                        const double rx = rng.nextFloat01();
                        const double ry = rng.nextFloat01();
                        const double rz = rng.nextFloat01();
                        // Always consume intensity rng step for determinism,
                        // even for stars culled below.
                        const float intensity = rng.nextFloat01();

                        const double x = sx + rx * S;
                        const double y = sy + ry * S;
                        const double z = sz + rz * S;

                        // Cull against the query AABB (doesn't affect determinism)
                        if (x < aabb.min.x || x > aabb.max.x) continue;
                        if (y < aabb.min.y || y > aabb.max.y) continue;
                        if (z < aabb.min.z || z > aabb.max.z) continue;

                        // Cubic mapping biases most stars toward small radii;
                        // rare high-intensity stars become genuinely massive.
                        const float radiusKm = 5.0e4f + intensity * intensity * intensity * 1.0e6f;

                        // Stable per-star id from sector coords + index (no rng advance).
                        const Seed64 starId = sector_seed(
                            masterSeed, ix, iy, iz,
                            static_cast<Seed64>(n) ^ 0x7A4B3C2D1E0F9E8FULL);

                        out.stars.push_back(Star3{
                            glm::dvec3{ x, y, z },
                            radiusKm, intensity, starId
                        });
                    }
                }
            }
        }
    }

} // namespace Interstellar::Universe
