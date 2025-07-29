#pragma once

#include <cstdint>

namespace Interstellar::Input {

    /**
     * @brief Interface for managing keyboard input.
     *
     * Provides methods for current and historical key states,
     * supporting both real-time and edge-triggered input detection.
     *
     * @since 1.0
     */
    class IKeyboardManager {
    public:
        virtual ~IKeyboardManager() = default;

        /**
         * @brief Checks if a specific key is currently being held down.
         * 
         * @param key The key code (e.g., GLFW_KEY_W).
         * @return True if the key is down, false otherwise.
         * @since 1.0
         */
        virtual bool IsKeyDown(int key) const = 0;

        /**
         * @brief Checks if the key was pressed (transitioned from up to down) this frame.
         *
         * This is useful to single-action input (e.g., jump, confirm).
         *
         * @param key The key code.
         * @return True if the key was just pressed this frame.
         * @since 1.0
         */
        virtual bool WasKeyPressed(int key) const = 0;

        /**
         * @brief Checks if the key was released (transitioned from down to up) this frame.
         *
         * @param key The key code.
         * @return True if the key was just released this frame.
         * @since 1.0
         */
        virtual bool WasKeyReleased(int key) const = 0;
    };
}
