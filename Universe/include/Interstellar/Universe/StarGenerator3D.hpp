#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Interstellar/Universe/Seed.hpp"
#include "Interstellar/Universe/Universe3DRecipe.hpp"
#include "Interstellar/Universe/Sector3.hpp"
#include "Interstellar/Universe/AABB.hpp"

namespace Interstellar::Universe {

    struct Star3 {
        glm::vec3 pos{};     // world position
        float     intensity; // arbitrary [0..1] brightness seed
        Seed64    id;        // stable unique id
    };

    // Generate deterministic stars for a given 3D sector; culled to optional AABB.
    void GenerateStarsInSector3(Seed64 masterSeed,
        const Universe3DRecipe& recipe,
        const Sector3Id& sector,
        std::vector<Star3>& outStars,
        const AABB3* aabbOpt = nullptr);

    // Generate deterministic stars for an arbitrary 3D AABB by walking overlapping sectors.
    void GenerateStarsInAABB3(Seed64 masterSeed,
        const Universe3DRecipe& recipe,
        const AABB3& aabb,
        std::vector<Star3>& outStars);

} // namespace Interstellar::Universe
