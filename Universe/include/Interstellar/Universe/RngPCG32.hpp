#pragma once
#include <cstdint>

namespace Interstellar::Universe {

    // Compact PCG32
    class RngPCG32 {
    public:
        explicit RngPCG32(uint64_t state = 0x853c49e6748fea9bULL, uint64_t seq = 0xda3e39cb94b95bdbULL) {
            seed(state, seq);
        }
        void seed(uint64_t state, uint64_t seq) {
            m_state = 0U; m_inc = (seq << 1u) | 1u; next(); m_state += state; next();
        }
        uint32_t next() {
            uint64_t oldstate = m_state;
            m_state = oldstate * 6364136223846793005ULL + m_inc;
            uint32_t xorshifted = static_cast<uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
            uint32_t rot = static_cast<uint32_t>(oldstate >> 59u);
            return (xorshifted >> rot) | (xorshifted << ((-static_cast<int>(rot)) & 31));
        }
        // 0..1
        float nextFloat() {
            return (next() >> 8) * (1.0f / 16777216.0f); // 24-bit mantissa
        }
    private:
        uint64_t m_state;
        uint64_t m_inc;
    };

} // namespace
