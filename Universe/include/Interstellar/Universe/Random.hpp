#pragma once
#include <cstdint>
#include "Interstellar/Universe/Seed.hpp"

namespace Interstellar::Universe {

    // mix64(Seed64) is declared/defined in Seed.hpp; we just use it here.

    // 64-bit hash helpers (deterministic, fast)
    inline std::uint64_t hash1_u64(std::uint64_t x) {
        std::uint64_t h = 0x9e3779b97f4a7c15ULL ^ x;
        return mix64(h);
    }

    inline std::uint64_t hash2_u64(std::uint64_t a, std::uint64_t b) {
        std::uint64_t h = 0x9e3779b97f4a7c15ULL;
        h = mix64(h ^ a);
        h = mix64(h ^ b);
        return h;
    }

    inline std::uint64_t hash3_u64(std::uint64_t a, std::uint64_t b, std::uint64_t c) {
        std::uint64_t h = 0xd1b54a32d192ed03ULL; // different offset basis
        h = mix64(h ^ a);
        h = mix64(h ^ b);
        h = mix64(h ^ c);
        return h;
    }

    // Tiny RNG using SplitMix64 progression (deterministic, fast)
    struct Rng64 {
        Seed64 state;
        explicit Rng64(Seed64 seed) : state(mix64(seed)) {}

        inline std::uint64_t nextU64() {
            state += 0x9e3779b97f4a7c15ULL;   // golden ratio increment
            return mix64(state);              // scramble
        }

        inline std::uint32_t nextU32() {
            return static_cast<std::uint32_t>(nextU64());
        }

        // [0,1) using 24 random bits (sufficient for GPU-ish uses)
        inline float nextFloat01() {
            return static_cast<float>((nextU64() >> 40) & 0xFFFFFFu) / 16777216.0f;
        }

        inline float uniform(float a, float b) {
            return a + (b - a) * nextFloat01();
        }
    };

} // namespace Interstellar::Universe
