#pragma once
/**
 * @file
 * @ingroup KeyCodes
 * @brief Platform-agnostic key codes used throughout the input system.
 * @since 1.0
 */

#include <cstdint>

namespace Interstellar {
    namespace Input {

        /**
         * @brief Unified key identifiers for cross-platform input.
         *
         * Notes:
         * - Values are not part of the ABI surface. Do not persist or serialize the
         *   underlying numeric values; use names.
         * - Use KeyCode::Unknown when a key cannot be mapped.
         * - Extend cautiously and keep names stable.
         *
         * @since 1.0
         */
        enum class KeyCode : std::uint16_t
        {
            Unknown = 0,            // Undefined or unmapped key.

            // Alphabet keys
            A, B, C, D, E, F, G, H, I, J, K, L, M,
            N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

            // Number row (not numpad)
            Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

            // Function keys
            F1, F2, F3, F4, F5, F6,
            F7, F8, F9, F10, F11, F12,
            // Optional extended function keys (keep for editor power-users)
            F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,

            // Modifiers
            LeftShift, RightShift,
            LeftControl, RightControl,
            LeftAlt, RightAlt,
            LeftSuper, RightSuper, // Windows key on Win, Command key on macOS

            // Arrow keys
            ArrowUp, ArrowDown, ArrowLeft, ArrowRight,

            // Navigation and editing
            Home, End, PageUp, PageDown, Insert, Delete,

            // Control keys
            Escape, Tab, Enter, Backspace, Space,

            // Lock keys
            CapsLock, ScrollLock, NumLock,

            // Punctuation and symbols (US keyboard set)
            Grave,            // Backquote/tilde key: ` (next to 1 on US layouts)
            Minus,            // -
            Equal,            // =
            LeftBracket,      // [
            RightBracket,     // ]
            Backslash,        // backslash '\'
            Semicolon,        // ;
            Apostrophe,       // '
            Comma,            // ,
            Period,           // .
            Slash,            // /

            // Special/system
            PrintScreen, Pause, Application, // Application/Menu key

            // Numpad (keypad)
            Kp0, Kp1, Kp2, Kp3, Kp4, Kp5, Kp6, Kp7, Kp8, Kp9,
            KpDecimal, KpDivide, KpMultiply, KpSubtract, KpAdd, KpEnter, KpEqual,

            // Sentinel (not a key): count of defined keys.
            Count
        };

        /**
         * @brief Lightweight helpers for common key queries.
         * @since 1.0
         */
        namespace KeyCodeUtil
        {
            inline bool IsModifier(KeyCode k) noexcept
            {
                switch (k)
                {
                case KeyCode::LeftShift:
                case KeyCode::RightShift:
                case KeyCode::LeftControl:
                case KeyCode::RightControl:
                case KeyCode::LeftAlt:
                case KeyCode::RightAlt:
                case KeyCode::LeftSuper:
                case KeyCode::RightSuper:
                    return true;
                default:
                    return false;
                }
            }

            inline bool IsFunctionKey(KeyCode k) noexcept
            {
                switch (k)
                {
                case KeyCode::F1:  case KeyCode::F2:  case KeyCode::F3:  case KeyCode::F4:
                case KeyCode::F5:  case KeyCode::F6:  case KeyCode::F7:  case KeyCode::F8:
                case KeyCode::F9:  case KeyCode::F10: case KeyCode::F11: case KeyCode::F12:
                case KeyCode::F13: case KeyCode::F14: case KeyCode::F15: case KeyCode::F16:
                case KeyCode::F17: case KeyCode::F18: case KeyCode::F19: case KeyCode::F20:
                case KeyCode::F21: case KeyCode::F22: case KeyCode::F23: case KeyCode::F24:
                    return true;
                default:
                    return false;
                }
            }

            inline bool IsArrowKey(KeyCode k) noexcept
            {
                switch (k)
                {
                case KeyCode::ArrowUp:
                case KeyCode::ArrowDown:
                case KeyCode::ArrowLeft:
                case KeyCode::ArrowRight:
                    return true;
                default:
                    return false;
                }
            }
        } // namespace KeyCodeUtil

    }
} // namespace Interstellar::Input
