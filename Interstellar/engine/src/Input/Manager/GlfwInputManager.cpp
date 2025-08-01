#include "Interstellar/Input/IInputManager.hpp"

#include "Input/Manager/GlfwInputManager.hpp"
#include "Input/Manager/GlfwInputManager.hpp"

namespace Interstellar::Input {
    namespace Manager {

        Interstellar::Input::Manager::GlfwInputManager::GlfwInputManager(GLFWwindow* window)
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
    }

    std::unique_ptr<IInputManager> IInputManager::Create(void* window) {
        return std::make_unique<Input::Manager::GlfwInputManager>(
            static_cast<GLFWwindow*>(window)
        );
    }
}

