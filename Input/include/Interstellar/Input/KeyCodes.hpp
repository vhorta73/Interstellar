#pragma once
#include <cstdint>

namespace Interstellar::Input {

    enum class KeyCode : uint16_t {
        Unknown = 0,

        // Control / Editing
        Escape, Enter, Tab, Backspace, Space,
        CapsLock, NumLock, ScrollLock,
        PrintScreen, Pause,

        // Modifiers
        LeftShift, RightShift,
        LeftControl, RightControl,
        LeftAlt, RightAlt,
        LeftSuper, RightSuper, // Win/Command
        Menu,

        // Navigation
        ArrowLeft, ArrowRight, ArrowUp, ArrowDown,
        Insert, Delete, Home, End, PageUp, PageDown,

        // Top number row (main keyboard)
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

        // Punctuation on main rowit status
        Minus, Equal,                // '-' and '='  (use Shift+'=' for '+')
        LeftBracket, RightBracket,   // '[' and ']'
        Semicolon, Apostrophe,       // ';' and '\''
        Comma, Period, Slash,        // ',' '.' '/'
        Backslash, GraveAccent,      // '\\' and '`'  (tilde is Shift+'`')

        // Letters
        A, B, C, D, E, F, G, H, I, J,
        K, L, M, N, O, P, Q, R, S, T,
        U, V, W, X, Y, Z,

        // Function keys
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,

        // Keypad
        KP0, KP1, KP2, KP3, KP4, KP5, KP6, KP7, KP8, KP9,
        KPDecimal, KPDivide, KPMultiply, KPSubtract, KPAdd, KPEnter, KPEqual,

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
