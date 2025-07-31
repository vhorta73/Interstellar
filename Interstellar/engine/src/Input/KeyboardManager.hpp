#pragma once

#include "Interstellar/Input/IKeyboardManager.hpp"
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace Interstellar::Input {

    /**
    * @ingroup EngineInterfaces
     * @brief Concrete implementation of IKeyboardManager using GLFW.
     *
     * Tracks key states across frames and enables queries for key down, press, and release.
     *
     * @since 1.0
     */
    class KeyboardManager : public IKeyboardManager {
    public:
        /**
         * @brief Constructs the keyboard manager with a GLFW window context.
         * @param window Valid pointer to the GLFW window instance for which to track keyboard input.
         * @since 1.0
         */
        explicit KeyboardManager(GLFWwindow* window);

        /**
         * @brief Destructor.
         * @since 1.0
         */
        ~KeyboardManager() override = default;

        // Non-copyable.
        KeyboardManager(const KeyboardManager&) = delete;             ///< Copy constructor deleted.
        KeyboardManager& operator=(const KeyboardManager&) = delete;  ///< Copy assignment deleted.

        // Movable.
        KeyboardManager(KeyboardManager&&) = default;                 ///< Move constructor defaulted.
        KeyboardManager& operator=(KeyboardManager&&) = default;      ///< Move assignment defaulted.


        /**
         * @brief Updates the internal state of keys.
         *
         * Should be called once per frame **after** glfwPollEvents() to detect key transitions.
         *
         * @since 1.0
         */
        void Update();

        /**
         * @brief Returns true if the specified key is currently held down.
         *
         * @param key GLFW key code (e.g., GLFW_KEY_A).
         * @return True if the key is down.
         * @since 1.0
         * 
         * @todo Abstract `int key` into a platform-agnostic KeyCode enum.
         */
        [[nodiscard]] bool IsKeyDown(int key) const override;

        /**
         * @brief Returns true if the key transitioned from up to down this frame.
         *
         * @param key GLFW key code.
         * @return True if just pressed.
         * @since 1.0
         * 
         * @todo Abstract `int key` into a platform-agnostic KeyCode enum.
         */
        [[nodiscard]] bool WasKeyPressed(int key) const override;

        /**
         * @brief Checks if the key was released this frame (i.e., transitioned from down to up).
         *
         * @param key GLFW key code.
         * @return True if just released.
         * @since 1.0
         * 
         * @todo Abstract `int key` into a platform-agnostic KeyCode enum.
         */
        [[nodiscard]] bool WasKeyReleased(int key) const override;

    private:
        GLFWwindow* m_Window;

        /**
         * @note Key state is tracked using hash maps, which scale well for moderate input volumes.
         * Key state is tracked across frames.
         */
        std::unordered_map<int, bool> m_CurrentState;
        std::unordered_map<int, bool> m_PreviousState;
    };
}