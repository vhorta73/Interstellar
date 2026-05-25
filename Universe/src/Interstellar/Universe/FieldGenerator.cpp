#include "Interstellar/Universe/FieldGenerator.hpp"
#include "Interstellar/Universe/Random.hpp" // hash3_u64()
#include <cmath>

namespace Interstellar::Universe {

    void FieldGenerator::generate(const AABB2& rect, std::vector<float>& outXY) const {
        outXY.clear();
        if (m_grid <= 0.0f) return;

        // integer cell range covering rect (pad 1 to catch jittered points near edges)
        const int ix0 = static_cast<int>(std::floor(rect.min.x / m_grid)) - 1;
        const int ix1 = static_cast<int>(std::ceil(rect.max.x / m_grid)) + 1;
        const int iy0 = static_cast<int>(std::floor(rect.min.y / m_grid)) - 1;
        const int iy1 = static_cast<int>(std::ceil(rect.max.y / m_grid)) + 1;

        outXY.reserve(static_cast<size_t>(ix1 - ix0 + 1) * static_cast<size_t>(iy1 - iy0 + 1) * 2);

        for (int cy = iy0; cy <= iy1; ++cy) {
            for (int cx = ix0; cx <= ix1; ++cx) {
                const float baseX = cx * m_grid;
                const float baseY = cy * m_grid;

                // Deterministic per-cell jitter in [-0.5, 0.5]
                const uint64_t cellSeed = hash3_u64(static_cast<uint64_t>(cx),
                    static_cast<uint64_t>(cy),
                    static_cast<uint64_t>(m_seed));

                const float jx = (float)((cellSeed >> 12) & 0xFFFF) / 65535.0f - 0.5f;
                const float jy = (float)((cellSeed >> 28) & 0xFFFF) / 65535.0f - 0.5f;

                const float x = baseX + jx * m_jitter * m_grid;
                const float y = baseY + jy * m_jitter * m_grid;

                if (x >= rect.min.x && x <= rect.max.x && y >= rect.min.y && y <= rect.max.y) {
                    outXY.push_back(x);
                    outXY.push_back(y);
                }
            }
        }
    }

} // namespace Interstellar::Universe
