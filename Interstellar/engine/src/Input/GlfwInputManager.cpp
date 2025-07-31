#include "GlfwInputManager.hpp"

namespace Interstellar::Input {

    GlfwInputManager::GlfwInputManager(GLFWwindow* window)
        : m_Mouse(window), m_Keyboard(window) {
    }

    IMouseManager& GlfwInputManager::GetMouseManager() {
        return m_Mouse;
    }

    IKeyboardManager& GlfwInputManager::GetKeyboardManager() {
        return m_Keyboard;
    }

    void GlfwInputManager::Update() {
        m_Mouse.Update();
        m_Keyboard.Update();
    }

    void GlfwInputManager::InjectSyntheticKeyPress(int /*key*/) {
        // @todo Implement actual synthetic injection logic.
    }

    void GlfwInputManager::ReloadConfig() {
        // @todo Reload bindings/sensitivity/etc from file.
    }

    std::unique_ptr<IInputManager> IInputManager::Create(void* window) {
        return std::make_unique<GlfwInputManager>(
            static_cast<GLFWwindow*>(window)
        );
    }

}
