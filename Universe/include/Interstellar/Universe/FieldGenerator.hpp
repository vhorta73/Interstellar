#pragma once
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include "Interstellar/Universe/AABB.hpp"  // AABB2
#include "Interstellar/Universe/Seed.hpp"  // Seed64

namespace Interstellar::Universe {

    class FieldGenerator {
    public:
        // grid: lattice spacing (world units)
        // jitter: 0..1 range (fraction of grid) for per-cell random offset
        FieldGenerator(Seed64 seed, float grid, float jitter)
            : m_seed(seed), m_grid(grid), m_jitter(jitter) {
        }

        void   reseed(Seed64 s) { m_seed = s; }
        Seed64 seed()     const { return m_seed; }

        void   setGrid(float g) { m_grid = g; }
        float  grid()       const { return m_grid; }

        void   setJitter(float j) { m_jitter = j; }
        float  jitter()     const { return m_jitter; }

        // Fills outXY with x,y pairs (flattened) of points inside rect
        void generate(const AABB2& rect, std::vector<float>& outXY) const;

    private:
        Seed64 m_seed;  // master seed for determinism
        float  m_grid;  // lattice step
        float  m_jitter;// 0..1 fraction of grid (how far points can deviate inside a cell)
    };

} // namespace Interstellar::Universe
