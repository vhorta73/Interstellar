#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "Interstellar/Universe/Seed.hpp"
#include "Interstellar/Universe/AABB.hpp"
#include "Interstellar/Universe/Universe3DRecipe.hpp"
#include "Interstellar/Universe/UniverseQuery.hpp"   // Star3, QueryUniverseAABB3

namespace Interstellar::Universe {

    class UniverseService {
    public:
        UniverseService(Seed64 seed, Universe3DRecipe recipe)
            : m_seed(seed), m_recipe(recipe) {
        }

        void setSeed(Seed64 s) { m_seed = s; }
        void setRecipe(const Universe3DRecipe& r) { m_recipe = r; }

        Seed64 seed() const { return m_seed; }
        const Universe3DRecipe& recipe() const { return m_recipe; }

        // Query stars visible in a perspective slice centered around camPos.z.
        // fovDeg/aspect define the frustum; we build a conservative "slab" AABB.
        void queryVisibleStars(const glm::vec3& camPos,
            float fovDeg,
            float aspect,
            std::vector<Star3>& outStars,
            float minSlabHalf = 50.0f,
            float slabScale = 0.08f) const
        {
            // Frustum-based XY half-extents at current depth
            const float halfH = std::tan(glm::radians(fovDeg * 0.5f)) * camPos.z;
            const float halfW = halfH * aspect;

            // Thickness scales with distance to reduce popping as you travel
            const float slabHalf = std::max(minSlabHalf, camPos.z * slabScale);

            AABB3 slab{
                glm::vec3(camPos.x - halfW, camPos.y - halfH, camPos.z - slabHalf),
                glm::vec3(camPos.x + halfW, camPos.y + halfH, camPos.z + slabHalf)
            };

            UniverseQueryFilters filters; // stars=true
            UniverseQueryResult result;
            result.clear();
            QueryUniverseAABB3(m_seed, m_recipe, slab, filters, result);
            outStars = std::move(result.stars);
        }

    private:
        Seed64 m_seed;
        Universe3DRecipe m_recipe;
    };

} // namespace Interstellar::Universe
