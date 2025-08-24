#pragma once
#include <cstdint>
#include <vector>
#include <glm/vec2.hpp>

namespace Interstellar::Universe {

    struct AABB2 { glm::vec2 min, max; };

    /// Very small, deterministic point field for prototyping.
    /// Generates one pseudo-random point per grid cell inside a world-space AABB.
    class FieldGenerator {
    public:
        explicit FieldGenerator(uint64_t masterSeed, float grid = 0.12f, float jitter = 0.45f)
            : m_seed(masterSeed), m_grid(grid), m_jitter(jitter) {
        }

        /// Fills 'out' with XY pairs (flattened floats) in world-space.
        /// Each visible grid cell contributes exactly one point, jittered deterministically.
        void generate(const AABB2& rect, std::vector<float>& out) const;

        float gridSize() const { return m_grid; }

    private:
        static inline uint64_t mix64(uint64_t x) {
            x ^= x >> 30; x *= 0xbf58476d1ce4e5b9ULL;
            x ^= x >> 27; x *= 0x94d049bb133111ebULL;
            x ^= x >> 31; return x;
        }
        static inline uint64_t hash2(int32_t x, int32_t y, uint64_t seed) {
            uint64_t h = seed ^ 0x9e3779b97f4a7c15ULL;
            h ^= mix64(static_cast<uint64_t>(x) + 0x632BE59BD9B4E019ULL) + (h << 6) + (h >> 2);
            h ^= mix64(static_cast<uint64_t>(y) + 0xAE985BF2CB60B88DULL) + (h << 6) + (h >> 2);
            return mix64(h);
        }

        uint64_t m_seed;
        float    m_grid;
        float    m_jitter; // 0..0.5-ish of cell size
    };

} // namespace Interstellar::Universe
