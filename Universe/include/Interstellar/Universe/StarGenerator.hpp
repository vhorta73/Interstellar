#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Interstellar/Universe/Seed.hpp"
#include "Interstellar/Universe/UniverseRecipe.hpp"
#include "Interstellar/Universe/Sector.hpp"

namespace Interstellar::Universe {

    // Generate deterministic stars for a given sector; culled to optional AABB.
    void GenerateStarsInSector(Seed64 masterSeed,
        const UniverseRecipe& recipe,
        const SectorId& sector,
        std::vector<glm::vec2>& outStars,
        const glm::vec2* aabbMin = nullptr,
        const glm::vec2* aabbMax = nullptr);

    // Generate deterministic stars for an arbitrary AABB by walking overlapping sectors.
    void GenerateStarsInAABB(Seed64 masterSeed,
        const UniverseRecipe& recipe,
        const glm::vec2& aabbMin,
        const glm::vec2& aabbMax,
        std::vector<glm::vec2>& outStars);

} // namespace Interstellar::Universe
