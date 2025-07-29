#pragma once

#include <cstdint>

namespace Interstellar::Input {

    /**
     * @brief Interface for managing mouse input.
     *
     * Provides methods for tracking position, button states, and dragging logic.
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
        virtual double GetX() const = 0;

        /**
         * @brief Returns the current Y position of the mouse in screen coordinates.
         * 
         * @return Mouse Y position as a double.
         * @since 1.0
         */
        virtual double GetY() const = 0;

        /**
         * @brief Checks if a specific mouse button is currently pressed.
         * 
         * @param button The button index (e.g., 0 = left, 1 = right, 2 = middle).
         * @return True if the button is pressed, false otherwise.
         * @since 1.0
         */
        virtual bool IsButtonPressed(int button) const = 0;

        /**
         * @brief Checks if the user is currently dragging.
         * 
         * A drag usually means the user is holding a button and moving the mouse.
         * 
         * @return True if dragging, false otherwise.
         * @since 1.0
         */
        virtual bool IsDragging() const = 0;
    };

}
