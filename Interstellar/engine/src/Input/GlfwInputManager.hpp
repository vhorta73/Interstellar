#pragma once

#include "InputManager.hpp"
#include "KeyboardManager.hpp"
#include "MouseManager.hpp"

struct GLFWwindow;

namespace Interstellar::Input {

    /**
     * @ingroup PlatformImplementations
     * @brief GLFW-backed implementation of the InputManager.
     *
     * Holds instances of KeyboardManager and MouseManager,
     * and delegates interface calls to them.
     *
     * @since 1.0
     */
    class GlfwInputManager : public InputManager {
    public:
        explicit GlfwInputManager(GLFWwindow* window);

        IMouseManager& GetMouseManager() override;
        IKeyboardManager& GetKeyboardManager() override;
        void Update() override;
        void InjectSyntheticKeyPress(int key) override;
        void ReloadConfig() override;

    private:
        MouseManager m_Mouse;
        KeyboardManager m_Keyboard;
    };

}
