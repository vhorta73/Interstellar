#include "Interstellar/Universe/FieldGenerator.hpp"
#include <cmath>

namespace Interstellar::Universe {

    void FieldGenerator::generate(const AABB2& rect, std::vector<float>& out) const {
        out.clear();
        if (rect.max.x <= rect.min.x || rect.max.y <= rect.min.y) return;

        const int minX = static_cast<int>(std::floor(rect.min.x / m_grid));
        const int maxX = static_cast<int>(std::floor(rect.max.x / m_grid));
        const int minY = static_cast<int>(std::floor(rect.min.y / m_grid));
        const int maxY = static_cast<int>(std::floor(rect.max.y / m_grid));

        const size_t estCount = static_cast<size_t>(std::max(0, maxX - minX + 1)) *
            static_cast<size_t>(std::max(0, maxY - minY + 1));
        out.reserve(out.size() + estCount * 2);

        for (int gy = minY; gy <= maxY; ++gy) {
            for (int gx = minX; gx <= maxX; ++gx) {
                uint64_t h = hash2(gx, gy, m_seed);
                // two 32-bit pseudo-randoms from h (no std RNG to keep it tiny & deterministic)
                uint32_t r1 = static_cast<uint32_t>(h);
                uint32_t r2 = static_cast<uint32_t>(h >> 32);

                // [-0.5, +0.5) jitter scaled by m_jitter * grid
                const float jx = (static_cast<float>(r1) / 4294967296.0f - 0.5f) * (m_jitter * 2.0f) * m_grid;
                const float jy = (static_cast<float>(r2) / 4294967296.0f - 0.5f) * (m_jitter * 2.0f) * m_grid;

                const float wx = gx * m_grid + jx;
                const float wy = gy * m_grid + jy;

                out.push_back(wx);
                out.push_back(wy);
            }
        }
    }

} // namespace Interstellar::Universe
