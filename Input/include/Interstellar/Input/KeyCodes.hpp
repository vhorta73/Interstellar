#pragma once
#include <cstdint>

namespace Interstellar::Input {

    enum class KeyCode : uint16_t {
        Unknown = 0,

        // Control
        Escape, Enter, Tab, Backspace, Space,

        // Arrows
        ArrowLeft, ArrowRight, ArrowUp, ArrowDown,

        // Digits
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

        // Letters
        A, B, C, D, E, F, G, H, I, J,
        K, L, M, N, O, P, Q, R, S, T,
        U, V, W, X, Y, Z,

        // Function keys (subset)
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        _Count
    };

    /**
     * @ingroup Input
     * @brief Get the number of defined key codes.
     * @return std::size_t - Count of KeyCode values.
     * @throws None
     * @complexity O(1)
     * @since 1.0
     */
    constexpr inline std::size_t KeyCodeCount() {
        return static_cast<std::size_t>(KeyCode::_Count);
    }

} // namespace Interstellar::Input
