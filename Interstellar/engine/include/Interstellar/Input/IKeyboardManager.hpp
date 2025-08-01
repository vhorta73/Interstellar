#pragma once

#include <cstdint>

namespace Interstellar::Input {

    /**
     * @ingroup GameInterfaces
     * @brief Interface for managing keyboard input.
     *
     * Provides methods for querying both real-time (held) and edge-triggered (pressed/released)
     * key states. Designed for high-level game logic to remain agnostic to platform input details.
     *
     * @since 1.0
     */
    class IKeyboardManager {
    public:
        virtual ~IKeyboardManager() = default;

        /**
         * @brief Checks if a specific key is currently being held down.
         *
         * @param key The platform-specific key code (e.g., GLFW_KEY_W).
         * @return True if the key is held down; false otherwise.
         * @since 1.0
         * 
         * @todo Replace raw `int` key codes with an engine-defined `KeyCode` enum
         *       to decouple game logic from platform-specific input backends.
         */
        virtual bool IsKeyDown(int key) const = 0;

        /**
         * @brief Checks if the given key is currently not held down.
         *
         * @param key The platform-specific key code (e.g., GLFW_KEY_W).
         * @return True if the key is up; false otherwise.
         * @since 1.0
         * 
         * @todo Replace raw `int` key codes with an engine-defined `KeyCode` enum
         *       to decouple game logic from platform-specific input backends.
         */
        virtual bool IsKeyUp(int key) const {
            return !IsKeyDown(key);
        }

        /**
         * @brief Checks if a key was pressed (transitioned from up to down) during this frame.
         *
         * Use this for one-time actions like jumping, confirming, or firing.
         *
         * @param key The platform-specific key code.
         * @return True if the key was pressed this frame.
         * @since 1.0
         * 
         * @todo Replace raw `int` key codes with an engine-defined `KeyCode` enum
         *       to decouple game logic from platform-specific input backends.
         */
        virtual bool WasKeyPressed(int key) const = 0;

        /**
         * @brief Checks if a key was released (transitioned from down to up) during this frame.
         *
         * @param key The platform-specific key code.
         * @return True if the key was released this frame.
         * @since 1.0
         * 
         * @todo Replace raw `int` key codes with an engine-defined `KeyCode` enum
         *       to decouple game logic from platform-specific input backends.
         */
        virtual bool WasKeyReleased(int key) const = 0;
    };
}
