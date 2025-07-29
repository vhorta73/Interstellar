#include "Interstellar/Input/IInputManager.hpp"
#include "Interstellar/Input/IMouseManager.hpp"
#include "Interstellar/Input/IKeyboardManager.hpp"
#include "KeyboardManager.hpp"
#include "MouseManager.hpp"

#include <GLFW/glfw3.h>

namespace Interstellar::Input {

    /**
     * @brief Concrete implementation of IInputManager for GLFW.
     *
     * Holds instances of KeyboardManager and MouseManager,
     * and delegates interface calls to them.
     *
     * @since 1.0
     */
    class InputManagerImpl : public IInputManager {
    public:
        explicit InputManagerImpl(GLFWwindow* window)
            : m_Mouse(window), m_Keyboard(window) {
        }

        IMouseManager& GetMouseManager() override { return m_Mouse; }
        IKeyboardManager& GetKeyboardManager() override { return m_Keyboard; }

        /**
         * @brief Updates the state of both mouse and keyboard.
         *
         * Call once per frame.
         *
         * @since 1.0
         */
        void Update() override {
            m_Mouse.Update();
            m_Keyboard.Update();
        }

    private:
        MouseManager m_Mouse;
        KeyboardManager m_Keyboard;
    };

    /**
     * @brief Factory method to create an input manager tied to a GLFW window.
     *
     * @param window Must be a valid GLFWwindow*.
     * @return A unique pointer to the created input manager instance.
     *
     * @since 1.0
     */
    std::unique_ptr<IInputManager> IInputManager::Create(void* window) {
        return std::make_unique<InputManagerImpl>(
            static_cast<GLFWwindow*>(window)
        );
    }
}
