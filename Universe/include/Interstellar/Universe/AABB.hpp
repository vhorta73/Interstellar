#pragma once
#include <glm/glm.hpp>

namespace Interstellar::Universe {

    struct AABB2 {
        glm::vec2 min{}, max{};
    };
    struct AABB3 {
        glm::vec3 min{}, max{};
    };

    inline bool contains(const AABB3& b, const glm::vec3& p) {
        return (p.x >= b.min.x && p.y >= b.min.y && p.z >= b.min.z &&
            p.x <= b.max.x && p.y <= b.max.y && p.z <= b.max.z);
    }

    inline AABB3 make_aabb3(const glm::vec3& a, const glm::vec3& b) {
        return { glm::min(a, b), glm::max(a, b) };
    }

} // namespace Interstellar::Universe
