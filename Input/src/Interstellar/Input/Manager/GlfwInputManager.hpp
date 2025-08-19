#pragma once
/**
 * @file
 * @brief GLFW-backed implementation of the engine input manager.
 * @ingroup PlatformImplementations
 * @since 1.0
 */

#include "Interstellar/Input/InternalInputManager.hpp"
#include "Interstellar/Input/KeyCode.hpp"
#include "Interstellar/Input/KeyCodeTranslator/GlfwKeyMap.hpp"
#include "Interstellar/Input/Keyboard/KeyboardManager.hpp"
#include "Interstellar/Input/Mouse/GlfwMouseManager.hpp"

 // Do not include <GLFW/glfw3.h> here; keep header light.
struct GLFWwindow;

namespace Interstellar {
    namespace Input {
        namespace Manager {

            /**
             * @brief GLFW-backed input manager that aggregates concrete keyboard and mouse managers.
             *
             * Contract:
             * - Call Update() once per frame after the window event pump has been serviced.
             * - Wire GLFW callbacks to the contained managers. For keys, forward to
             *   Keyboard::KeyboardManager::OnGlfwKey(...). For mouse, forward to the mouse manager hooks.
             * - Window pointer is non-owning and must remain valid for this manager's lifetime.
             *
             * @since 1.0
             */
            class GlfwInputManager final : public InternalInputManager {
            public:
                /**
                 * @brief Construct with a valid GLFW window pointer.
                 * @param window Non-null pointer to GLFWwindow.
                 * @since 1.0
                 */
                explicit GlfwInputManager(GLFWwindow* window) noexcept;

                ~GlfwInputManager() noexcept override = default;

                GlfwInputManager(const GlfwInputManager&) = delete;
                GlfwInputManager& operator=(const GlfwInputManager&) = delete;
                GlfwInputManager(GlfwInputManager&&) = delete;
                GlfwInputManager& operator=(GlfwInputManager&&) = delete;

                // IInputManager
                [[nodiscard]] IMouseManager& GetMouseManager() noexcept override;
                [[nodiscard]] const IMouseManager& GetMouseManager() const noexcept override;

                [[nodiscard]] IKeyboardManager& GetKeyboardManager() noexcept override;
                [[nodiscard]] const IKeyboardManager& GetKeyboardManager() const noexcept override;

                void Update() noexcept override;

                // InternalInputManager
                void InjectSyntheticKeyPress(KeyCode key) noexcept override;
                void ReloadConfig() noexcept override;

            private:
                GLFWwindow* m_Window; // not owned

                // Translator must outlive the managers that use it.
                KeyCodeTranslator::GLFWKeyMap m_KeyTranslator;

                // Concrete device managers
                Mouse::GlfwMouseManager m_Mouse;
                Keyboard::KeyboardManager m_Keyboard;
            };

        }
    }
} // namespace Interstellar::Input::Manager
