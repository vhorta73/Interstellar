#pragma once
#include <cstdint>

namespace Interstellar::Universe {

    // integer sector coords (e.g., 1024x1024 world units per sector)
    struct SectorCoord {
        int64_t x{};
        int64_t y{};
    };

    // simple 64-bit mix for per-sector seed derivation
    inline uint64_t mix64(uint64_t x) {
        x ^= x >> 33; x *= 0xff51afd7ed558ccdULL;
        x ^= x >> 33; x *= 0xc4ceb9fe1a85ec53ULL;
        x ^= x >> 33; return x;
    }

    inline uint64_t deriveSectorSeed(uint64_t masterSeed, SectorCoord s) {
        uint64_t h = masterSeed ^ mix64(static_cast<uint64_t>(s.x)) ^ mix64(static_cast<uint64_t>(s.y) + 0x9e3779b97f4a7c15ULL);
        return mix64(h);
    }

} // namespace
