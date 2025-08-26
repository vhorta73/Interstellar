#pragma once
#include <cstdint>
#include <cmath>
#include <limits>
#include "Interstellar/Universe/Seed.hpp"   // mix64, Seed64

namespace Interstellar::Universe {

    // Robust floor division to an int64 sector index: floor(value / cell)
    inline std::int64_t floor_div_to_i64(double value, double cell) {
        if (cell == 0.0) return 0; // guard; shouldn't happen
        double q = std::floor(value / cell);
        // Clamp to int64 range (extremely large coordinates)
        if (q < static_cast<double>(std::numeric_limits<std::int64_t>::min()))
            return std::numeric_limits<std::int64_t>::min();
        if (q > static_cast<double>(std::numeric_limits<std::int64_t>::max()))
            return std::numeric_limits<std::int64_t>::max();
        return static_cast<std::int64_t>(q);
    }

    // Simple 64-bit hash combine using splitmix64-style mixing
    inline Seed64 hash_combine64(Seed64 h, Seed64 k) {
        // mix the incoming key
        k = mix64(k + 0x9e3779b97f4a7c15ull);
        // combine then re-mix
        h ^= k + 0x9e3779b97f4a7c15ull + (h << 6) + (h >> 2);
        return mix64(h);
    }

    // Hash for a 3D integer grid index (+ optional salt)
    inline Seed64 hash3_64(std::int64_t ix, std::int64_t iy, std::int64_t iz, Seed64 salt = 0) {
        Seed64 h = mix64(static_cast<Seed64>(ix) ^ 0xbf58476d1ce4e5b9ull);
        h = hash_combine64(h, static_cast<Seed64>(iy) ^ 0x94d049bb133111ebull);
        h = hash_combine64(h, static_cast<Seed64>(iz) ^ 0x2545f4914f6cdd1dull);
        if (salt) h = hash_combine64(h, salt);
        return h;
    }

    // Deterministic per-sector seed (depends only on master + sector coords [+ salt])
    inline Seed64 sector_seed(Seed64 master, std::int64_t ix, std::int64_t iy, std::int64_t iz, Seed64 salt = 0) {
        // mix master once so nearby masters produce different universes
        const Seed64 m = mix64(master ^ 0xa5a5a5a5a5a5a5a5ull);
        return mix64(hash3_64(ix, iy, iz, salt ^ m) ^ m);
    }

} // namespace Interstellar::Universe
