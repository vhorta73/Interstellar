// File: src/Interstellar/Input/Keyboard/KeyboardManager.cpp

#include "Interstellar/Input/Keyboard/KeyboardManager.hpp"

#include <GLFW/glfw3.h>
#include <cstddef> // for std::size_t
#include <cstring> // optional if you later use memcpy

namespace Interstellar {
    namespace Input {
        namespace Keyboard {

            static inline std::size_t ToIndex(KeyCode k) noexcept
            {
                return static_cast<std::size_t>(k);
            }

            KeyboardManager::KeyboardManager(GLFWwindow* window, const IKeyCodeTranslator& translator) noexcept
                : m_Window(window)
                , m_Translator(translator)
            {
                // Buffers default-initialized to zero.
            }

            void KeyboardManager::Update() noexcept
            {
                // Publish next frame state and advance edges:
                // previous <- current, current <- next
                m_Previous = m_Current;
                m_Current = m_Next;

                // Note: we purposely do NOT clear m_Next here.
                // Callbacks should keep m_Next accurate throughout the frame.
                // If you ever switch to a polling backend, you may choose to rebuild m_Next each frame.
            }

            bool KeyboardManager::IsKeyDown(KeyCode key) const noexcept
            {
                if (key == KeyCode::Unknown) return false;
                const std::size_t idx = ToIndex(key);
                return (idx < KeyCount) && (m_Current[idx] != 0);
            }

            bool KeyboardManager::WasKeyPressed(KeyCode key) const noexcept
            {
                if (key == KeyCode::Unknown) return false;
                const std::size_t idx = ToIndex(key);
                if (idx >= KeyCount) return false;
                const bool curr = m_Current[idx] != 0;
                const bool prev = m_Previous[idx] != 0;
                return curr && !prev;
            }

            bool KeyboardManager::WasKeyReleased(KeyCode key) const noexcept
            {
                if (key == KeyCode::Unknown) return false;
                const std::size_t idx = ToIndex(key);
                if (idx >= KeyCount) return false;
                const bool curr = m_Current[idx] != 0;
                const bool prev = m_Previous[idx] != 0;
                return !curr && prev;
            }

            void KeyboardManager::OnGlfwKey(int key, int scancode, int action, int mods) noexcept
            {
                (void)scancode;
                (void)mods;

                const KeyCode code = TranslateFromGlfw(key);
                if (code == KeyCode::Unknown) return;

                const std::size_t idx = ToIndex(code);
                if (idx >= KeyCount) return;

                // Events write into m_Next; Update() will publish to m_Current.
                switch (action)
                {
                case GLFW_PRESS:
                case GLFW_REPEAT: // repeat keeps key down; does not retrigger edge next Update
                    m_Next[idx] = 1u;
                    break;
                case GLFW_RELEASE:
                    m_Next[idx] = 0u;
                    break;
                default:
                    break;
                }
            }

            KeyCode KeyboardManager::TranslateFromGlfw(int glfwKey) const noexcept
            {
                return m_Translator.FromPlatform(glfwKey);
            }

            void KeyboardManager::OnFocusLost() noexcept
            {
                // Clear all three buffers so no edges fire spuriously when focus returns.
                m_Previous.fill(0);
                m_Current.fill(0);
                m_Next.fill(0);
            }


        }
    }
} // namespace Interstellar::Input::Keyboard
