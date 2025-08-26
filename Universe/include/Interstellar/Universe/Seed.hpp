#pragma once
#include <cstdint>
#include <string_view>

namespace Interstellar::Universe {

    using Seed64 = std::uint64_t;

    // SplitMix64 mixer (good avalanche, cheap)
    constexpr inline Seed64 mix64(Seed64 x) {
        x += 0x9e3779b97f4a7c15ull;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
        return x ^ (x >> 31);
    }

    constexpr inline Seed64 hashCombine(Seed64 a, Seed64 b) {
        return mix64(a ^ mix64(b + 0x9e3779b97f4a7c15ull));
    }

    constexpr inline Seed64 hashInts(std::int64_t x, std::int64_t y) {
        return hashCombine(mix64(static_cast<Seed64>(x)), mix64(static_cast<Seed64>(y)));
    }

    // FNV-1a 64 for user strings -> seeds
    inline Seed64 seedFromString(std::string_view s) {
        Seed64 h = 1469598103934665603ull;
        for (unsigned char c : s) {
            h ^= c;
            h *= 1099511628211ull;
        }
        return h;
    }

} // namespace Interstellar::Universe
