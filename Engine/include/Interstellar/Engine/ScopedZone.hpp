#pragma once
#include <chrono>
#include "Interstellar/Logging/Logging.hpp"

namespace Interstellar::Engine {

    class ScopedZone {
    public:
        ScopedZone(const char* label, bool enabled = true)
            : m_label(label), m_enabled(enabled), m_start(Clock::now()) {
        }
        ~ScopedZone() {
            if (!m_enabled) return;
            using namespace Interstellar::Logging;
            auto end = Clock::now();
            std::chrono::duration<double, std::milli> ms = end - m_start;
            LogInit().LogDebug("[Zone] {}: {:.3f} ms", m_label, ms.count());
        }
    private:
        using Clock = std::chrono::high_resolution_clock;
        const char* m_label;
        bool m_enabled;
        Clock::time_point m_start;
    };

} // namespace Interstellar::Engine
