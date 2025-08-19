#pragma once

#include <cstdint>

namespace Interstellar::Input {

    /**
     * @ingroup KeyCodes
     * @brief Platform-agnostic mouse button identifiers.
     *
     * Abstracts away platform-specific constants (e.g., GLFW_MOUSE_BUTTON_*).
     * Used across the input system to decouple engine logic from backend APIs.
     *
     * @since 1.0
     */
    enum class MouseButton : std::uint8_t {
        Unknown = 0,   ///< Undefined or unmapped button.

        Left,          ///< Primary button (usually left).
        Right,         ///< Secondary button (usually right).
        Middle,        ///< Middle / wheel button.

        Button4,       ///< Additional button 4.
        Button5,       ///< Additional button 5.
        Button6,       ///< Additional button 6.
        Button7,       ///< Additional button 7.
        Button8,       ///< Additional button 8.

        Count          ///< Sentinel for array sizing.
    };

} // namespace Interstellar::Input
