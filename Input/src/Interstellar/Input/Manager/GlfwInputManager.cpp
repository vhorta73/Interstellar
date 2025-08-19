// File: src/Interstellar/Input/Manager/GlfwInputManager.cpp

#include "Interstellar/Input/Manager/GlfwInputManager.hpp"
#include "Interstellar/Input/IInputManager.hpp"  // for factory definition

// GLFW only in .cpp
#include <GLFW/glfw3.h>

namespace Interstellar {
    namespace Input {
        namespace Manager {

            GlfwInputManager::GlfwInputManager(GLFWwindow* window) noexcept
                : m_Window(window)
                , m_KeyTranslator()
                , m_Mouse(window)                    // assumes GlfwMouseManager(window) ctor
                , m_Keyboard(window, m_KeyTranslator) // our KeyboardManager takes (GLFWwindow*, IKeyCodeTranslator&)
            {
            }

            IMouseManager& GlfwInputManager::GetMouseManager() noexcept {
                return m_Mouse;
            }

            const IMouseManager& GlfwInputManager::GetMouseManager() const noexcept {
                return m_Mouse;
            }

            IKeyboardManager& GlfwInputManager::GetKeyboardManager() noexcept {
                return m_Keyboard;
            }

            const IKeyboardManager& GlfwInputManager::GetKeyboardManager() const noexcept {
                return m_Keyboard;
            }

            void GlfwInputManager::Update() noexcept {
                // Expect that the application already called glfwPollEvents().
                m_Mouse.Update();
                m_Keyboard.Update();
            }

            void GlfwInputManager::InjectSyntheticKeyPress(KeyCode key) noexcept {
                // Synthesize a GLFW key press using the translator, then feed it to the keyboard manager.
                const int glfwKey = m_KeyTranslator.ToPlatform(key);
                if (glfwKey < 0) {
                    return; // unknown mapping, ignore
                }

                // Press event; caller can inject a release later if needed.
                // If you need an immediate click (press+release in one frame), call OnGlfwKey twice here.
                m_Keyboard.OnGlfwKey(glfwKey, /*scancode*/0, GLFW_PRESS, /*mods*/0);

                // Optional: uncomment to synthesize an immediate release in the same frame.
                // m_Keyboard.OnGlfwKey(glfwKey, /*scancode*/0, GLFW_RELEASE, /*mods*/0);
            }

            void GlfwInputManager::ReloadConfig() noexcept {
                // Stub: load bindings/sensitivity/platform overrides here.
                // Keep this idempotent and safe to call at runtime.
            }

        }
    }
} // namespace Interstellar::Input::Manager

// Factory on the interface
namespace Interstellar {
    namespace Input {

        std::unique_ptr<IInputManager> IInputManager::Create(void* window) noexcept {
            return std::make_unique<Manager::GlfwInputManager>(
                static_cast<GLFWwindow*>(window)
            );
        }

    }
} // namespace Interstellar::Input
