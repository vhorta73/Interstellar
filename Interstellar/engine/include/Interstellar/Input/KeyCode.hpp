#pragma once

#include <cstdint>

namespace Interstellar::Input {

    /**
     * @ingroup KeyCodes
     * @brief Platform-agnostic key codes used throughout the input system.
     *
     * This enum standardizes key identifiers across different platforms and APIs.
     * It is designed to decouple application logic from underlying input backends like GLFW, SDL, etc.
     *
     * KeyCode values should be translated internally by the input layer.
     *
     * @since 1.0
     */
    enum class KeyCode : std::uint16_t {

        Unknown = 0,               ///< Undefined or unmapped key.

        // Alphabet keys
        A,                         ///< The 'A' key.
        B,                         ///< The 'B' key.
        C,                         ///< The 'C' key.
        D,                         ///< The 'D' key.
        E,                         ///< The 'E' key.
        F,                         ///< The 'F' key.
        G,                         ///< The 'G' key.
        H,                         ///< The 'H' key.
        I,                         ///< The 'I' key.
        J,                         ///< The 'J' key.
        K,                         ///< The 'K' key.
        L,                         ///< The 'L' key.
        M,                         ///< The 'M' key.
        N,                         ///< The 'N' key.
        O,                         ///< The 'O' key.
        P,                         ///< The 'P' key.
        Q,                         ///< The 'Q' key.
        R,                         ///< The 'R' key.
        S,                         ///< The 'S' key.
        T,                         ///< The 'T' key.
        U,                         ///< The 'U' key.
        V,                         ///< The 'V' key.
        W,                         ///< The 'W' key.
        X,                         ///< The 'X' key.
        Y,                         ///< The 'Y' key.
        Z,                         ///< The 'Z' key.

        // Number row keys (not numpad)
        Num0,                      ///< The '0' key.
        Num1,                      ///< The '1' key.
        Num2,                      ///< The '2' key.
        Num3,                      ///< The '3' key.
        Num4,                      ///< The '4' key.
        Num5,                      ///< The '5' key.
        Num6,                      ///< The '6' key.
        Num7,                      ///< The '7' key.
        Num8,                      ///< The '8' key.
        Num9,                      ///< The '9' key.

        // Function keys
        F1,                        ///< Function key F1.
        F2,                        ///< Function key F2.
        F3,                        ///< Function key F3.
        F4,                        ///< Function key F4.
        F5,                        ///< Function key F5.
        F6,                        ///< Function key F6.
        F7,                        ///< Function key F7.
        F8,                        ///< Function key F8.
        F9,                        ///< Function key F9.
        F10,                       ///< Function key F10.
        F11,                       ///< Function key F11.
        F12,                       ///< Function key F12.

        // Modifier keys
        LeftShift,                ///< Left Shift key.
        RightShift,               ///< Right Shift key.
        LeftControl,              ///< Left Control key.
        RightControl,             ///< Right Control key.
        LeftAlt,                  ///< Left Alt (Option) key.
        RightAlt,                 ///< Right Alt (Option) key.

        // Arrow keys
        ArrowUp,                  ///< Up arrow key.
        ArrowDown,                ///< Down arrow key.
        ArrowLeft,                ///< Left arrow key.
        ArrowRight,               ///< Right arrow key.

        // Navigation & editing
        Home,                     ///< Home key.
        End,                      ///< End key.
        PageUp,                   ///< Page Up key.
        PageDown,                 ///< Page Down key.
        Insert,                   ///< Insert key.
        Delete,                   ///< Delete key.

        // Control keys
        Escape,                   ///< Escape key.
        Tab,                      ///< Tab key.
        Enter,                    ///< Enter (Return) key.
        Backspace,                ///< Backspace key.
        Space,                    ///< Spacebar.

        // Lock keys
        CapsLock,                 ///< Caps Lock key.
        ScrollLock,               ///< Scroll Lock key.
        NumLock,                  ///< Num Lock key.

        // Punctuation and symbols
        Minus,                    ///< Minus ('-') key.
        Equal,                    ///< Equal ('=') key.
        LeftBracket,              ///< Left square bracket ('[') key.
        RightBracket,             ///< Right square bracket (']') key.
        Backslash,                ///< Backslash ('\\') key.
        Semicolon,                ///< Semicolon (';') key.
        Apostrophe,               ///< Apostrophe (''') key.
        Comma,                    ///< Comma (',') key.
        Period,                   ///< Period ('.') key.
        Slash,                    ///< Slash ('/') key.

        // Special/system
        PrintScreen,              ///< Print Screen key.
        Pause,                    ///< Pause/Break key.
        Application               ///< Application/Menu key (usually between right Alt and Ctrl).
    };

} // namespace Interstellar::Input
