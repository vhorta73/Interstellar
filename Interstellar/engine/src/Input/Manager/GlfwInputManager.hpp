#pragma once

#include "Input/InternalInputManager.hpp"
#include "Input/Keyboard/KeyboardManager.hpp"
#include "Input/Mouse/GlfwMouseManager.hpp"

struct GLFWwindow;

namespace Interstellar::Input::Manager {

    /**
     * @ingroup PlatformImplementations
     * @brief GLFW-backed implementation of the InputManager.
     *
     * Holds instances of KeyboardManager and MouseManager,
     * and delegates interface calls to them.
     *
     * @since 1.0
     */
    class GlfwInputManager : public InternalInputManager {
    public:
        explicit GlfwInputManager(GLFWwindow* window);

        IMouseManager& GetMouseManager() override;
        IKeyboardManager& GetKeyboardManager() override;
        void Update() override;
        void InjectSyntheticKeyPress(int key) override;
        void ReloadConfig() override;

    private:
        Interstellar::Input::Mouse::GlfwMouseManager m_Mouse;
        Keyboard::KeyboardManager m_Keyboard;
    };

}
