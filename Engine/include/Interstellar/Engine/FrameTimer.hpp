#pragma once
#include <chrono>
#include <cstdint>
#include <algorithm>

namespace Interstellar::Engine {

    class FrameTimer {
    public:
        explicit FrameTimer(double targetHz = 0.0)  // 0 = variable dt
            : m_targetHz(targetHz),
            m_targetDt(targetHz > 0.0 ? 1.0 / targetHz : 0.0),
            m_last(Clock::now()) {
        }

        // returns dt (seconds). If fixed, may return m_targetDt repeatedly.
        double tick() {
            auto now = Clock::now();
            std::chrono::duration<double> diff = now - m_last;
            m_last = now;
            double dt = diff.count();
            if (m_targetHz <= 0.0) return dt; // variable

            m_accum += dt;
            // clamp to avoid spiral of death on long pauses
            m_accum = std::min(m_accum, 0.25);
            if (m_accum >= m_targetDt) {
                m_accum -= m_targetDt;
                return m_targetDt; // drive a fixed-step sim
            }
            // not enough time accumulated; tell caller to skip this sim step
            return -1.0;
        }

        void reset() { m_last = Clock::now(); m_accum = 0.0; }

    private:
        using Clock = std::chrono::high_resolution_clock;
        double m_targetHz;
        double m_targetDt;
        double m_accum = 0.0;
        Clock::time_point m_last;
    };

} // namespace Interstellar::Engine
