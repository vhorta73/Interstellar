#pragma once

#include "Interstellar/Input/IKeyboardManager.hpp"
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace Interstellar::Input {

    /**
     * @brief Concrete implementation of IKeyboardManager using GLFW.
     *
     * Tracks key states across frames, enabling queries for key down, press, and release.
     *
     * @since 1.0
     */
    class KeyboardManager : public IKeyboardManager {
    public:
        /**
         * @brief Constructs the keyboard manager with a GLFW window context.
         * @param window Pointer to the GLFW window to poll key states from.
         * @since 1.0
         */
        explicit KeyboardManager(GLFWwindow* window);

        /**
         * @brief Destructor.
         * @since 1.0
         */
        ~KeyboardManager() override = default;

        /**
         * @brief Updates the internal state of keys.
         *
         * Should be called once per frame to detect key transitions.
         *
         * @since 1.0
         */
        void Update();

        /**
         * @brief Checks if the given key is currently being held down.
         *
         * @param key GLFW key code (e.g., GLFW_KEY_A).
         * @return True if the key is down.
         * @since 1.0
         */
        bool IsKeyDown(int key) const override;

        /**
         * @brief Checks if the key was pressed this frame (i.e., transitioned from up to down).
         *
         * @param key GLFW key code.
         * @return True if just pressed.
         * @since 1.0
         */
        bool WasKeyPressed(int key) const override;

        /**
         * @brief Checks if the key was released this frame (i.e., transitioned from down to up).
         *
         * @param key GLFW key code.
         * @return True if just released.
         * @since 1.0
         */
        bool WasKeyReleased(int key) const override;

    private:
        GLFWwindow* m_Window;
        std::unordered_map<int, bool> m_CurrentState;
        std::unordered_map<int, bool> m_PreviousState;
    };
}