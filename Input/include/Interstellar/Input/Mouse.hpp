#pragma once
#include <algorithm>
#include <glm/glm.hpp>
#include <Interstellar/Input/IMouse.hpp>
#include <Interstellar/Input/MouseButtons.hpp>

namespace Interstellar::Input {

    /**
     * @ingroup Input
     * @brief Normalize mouse delta into approximate NDC space.
     * @details Converts pixel delta into range [-1..+1] across the window.
     * Y axis is up (negative sign applied).
     * @param m [in] const IMouse& - Mouse source.
     * @param windowW [in] int - Window width in pixels (> 0).
     * @param windowH [in] int - Window height in pixels (> 0).
     * @return glm::vec2 - Normalized delta; {0,0} if windowW or windowH <= 0.
     * @throws None
     * @complexity O(1)
     * @since 1.0
     */
    inline glm::vec2 DeltaNorm(const IMouse& m, int windowW, int windowH) {
        const auto d = m.delta();
        if (windowW <= 0 || windowH <= 0) return { 0.f, 0.f };
        return { static_cast<float>(d.dx / windowW * 2.0),
                -static_cast<float>(d.dy / windowH * 2.0) };
    }

    /**
     * @ingroup Input
     * @brief Convert wheel motion into a clamped zoom value.
     * @details Applies a linear step per wheel unit, then clamps to [minZ,maxZ].
     * Requires IMouse::wheel() to be implemented by backend.
     * @param m [in] const IMouse& - Mouse source.
     * @param current [in] float - Current zoom value.
     * @param step [in] float - Zoom change per wheel tick (default 0.5).
     * @param minZ [in] float - Minimum zoom (default 0.1).
     * @param maxZ [in] float - Maximum zoom (default 5000.0).
     * @return float - New clamped zoom value.
     * @throws None
     * @complexity O(1)
     * @since 1.0
     */
    inline float WheelZoom(const IMouse& m, float current,
        float step = 0.5f,
        float minZ = 0.1f,
        float maxZ = 5000.0f) {
        const auto wh = m.wheel();
        if (wh.y == 0.0f && wh.x == 0.0f) return current;
        return std::clamp(current + wh.y * step, minZ, maxZ);
    }

    struct MouseFilter {
        float smoothing = 0.35f; // 0 = no smoothing, 0.9 = heavy
        glm::vec2 prev{ 0.f };

        /**
         * @ingroup Input
         * @brief Apply exponential smoothing to normalized mouse delta.
         * @details new = prev * (1 - a) + n * a, where a = clamp(1 - smoothing, 0, 1).
         * Uses DeltaNorm() as the normalized delta source.
         * @param m [in] const IMouse& - Mouse source.
         * @param w [in] int - Window width in pixels.
         * @param h [in] int - Window height in pixels.
         * @return glm::vec2 - Smoothed normalized delta.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        glm::vec2 apply(const IMouse& m, int w, int h) {
            const glm::vec2 n = DeltaNorm(m, w, h);
            const float a = std::clamp(1.0f - smoothing, 0.0f, 1.0f);
            prev = prev * (1.0f - a) + n * a;
            return prev;
        }

        /**
         * @ingroup Input
         * @brief Reset the smoothing filter state.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        void reset() { prev = { 0.f, 0.f }; }
    };

} // namespace Interstellar::Input
