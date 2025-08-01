#pragma once

#include <cstdint>

namespace Interstellar::Input {

    /**
     * @ingroup GameInterfaces
     * @brief Interface for managing mouse input.
     *
     * Provides methods for tracking position, button states, and dragging logic.
     * Coordinates are typically in screen space (origin at top-left).
     *
     * @since 1.0
     */
    class IMouseManager {
    public:
        virtual ~IMouseManager() = default;

        /**
         * @brief Returns the current X position of the mouse in screen coordinates.
         * 
         * @return Mouse X position as a double.
         * @since 1.0
         */
        [[nodiscard]] virtual double GetX() const = 0;

        /**
         * @brief Returns the current Y position of the mouse in screen coordinates.
         * 
         * @return Mouse Y position as a double.
         * @since 1.0
         */
        [[nodiscard]] virtual double GetY() const = 0;

        /**
         * @brief Checks if a specific mouse button is currently pressed.
         * 
         * @param button The button index (e.g., 0 = left, 1 = right, 2 = middle). Platform-specific constants like GLFW_MOUSE_BUTTON_... may be used.

         * @return True if the button is pressed, false otherwise.
         * @since 1.0
         */
        [[nodiscard]] virtual bool IsButtonPressed(int button) const = 0;

        /**
         * @brief Checks if the user is currently dragging.
         * 
         * A drag usually means the user is holding a button (commonly the left button)
         * and moving the mouse.
         * 
         * @return True if dragging, false otherwise.
         * @since 1.0
         */
        [[nodiscard]] virtual bool IsDragging() const = 0;

        /**
        * @brief Returns the accumulated vertical scroll offset since the last update.
        * @return Y-axis scroll offset (positive = scroll up, negative = down).
        * 
        * @since 1.0
        */
        [[nodiscard]] virtual double GetScrollOffsetY() const = 0;

    };

}
