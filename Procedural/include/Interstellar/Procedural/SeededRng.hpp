#pragma once
#include <cstdint>
#include <string_view>
#include <limits>
#include <initializer_list>

namespace Interstellar::Procedural {

    // ---------------------------------------------
    // 64-bit FNV-1a (constexpr) for domain tags
    // ---------------------------------------------
    constexpr uint64_t fnv1a64(std::string_view s) {
        uint64_t h = 1469598103934665603ull;
        for (char c : s) {
            h ^= static_cast<unsigned char>(c);
            h *= 1099511628211ull;
        }
        return h;
    }

    // ---------------------------------------------
    // splitmix64 (seed mixer / stream splitter)
    // ---------------------------------------------
    inline uint64_t splitmix64(uint64_t& x) {
        uint64_t z = (x += 0x9E3779B97F4A7C15ull);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
        return z ^ (z >> 31);
    }

    // combine two 64-bit values with avalanche
    constexpr inline uint64_t mix64(uint64_t a, uint64_t b) {
        uint64_t x = a ^ (b + 0x9E3779B97F4A7C15ull + (a << 6) + (a >> 2));
        x ^= x >> 27; x *= 0x3C79AC492BA7B653ull;
        x ^= x >> 33; x *= 0x1C69B3F74AC4AE35ull;
        x ^= x >> 27;
        return x;
    }

    // ---------------------------------------------
    // Deterministic stream derivation
    // master -> domain -> path parts -> stream seed
    // ---------------------------------------------
    template <typename... Parts>
    inline uint64_t derive_stream_seed(uint64_t master, uint64_t domainTag, Parts... parts) {
        uint64_t h = mix64(master, domainTag);
        (void)std::initializer_list<int>{ (h = mix64(h, static_cast<uint64_t>(parts)), 0)... };
        uint64_t t = h;
        return splitmix64(t);
    }

    // ---------------------------------------------
    // Tiny deterministic RNG (based on splitmix64)
    // ---------------------------------------------
    class Rng64 {
        uint64_t state;
    public:
        explicit Rng64(uint64_t seed) : state(seed ? seed : 0xCAFEBABEDEADBEEFull) {}

        inline uint64_t u64() { return splitmix64(state); }
        inline uint32_t u32() { return static_cast<uint32_t>(u64() >> 32); }

        // double in [0,1)
        inline double uniform01() {
            return (u64() >> 11) * (1.0 / 9007199254740992.0);
        }

        // float in [min,max)
        inline float uniform(float min, float max) {
            return static_cast<float>(min + (max - min) * uniform01());
        }

        // integer in [min,max] inclusive
        inline int irange(int min, int max) {
            if (min > max) { int t = min; min = max; max = t; }
            uint32_t span = static_cast<uint32_t>(max - min + 1);
            return min + static_cast<int>(u32() % span);
        }
    };

} // namespace Interstellar::Procedural
