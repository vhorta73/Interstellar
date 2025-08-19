#pragma once
/**
 * @file
 * @brief GLFW-backed keyboard manager implementing IKeyboardManager.
 * @ingroup PlatformImplementations
 * @ingroup InputSystem
 * @since 1.0
 */

#include <array>
#include <cstdint>
#include "Interstellar/Input/IKeyboardManager.hpp"
#include "Interstellar/Input/KeyCode.hpp"
#include "Interstellar/Input/KeyCodeTranslator/IKeyCodeTranslator.hpp"

 // Forward declaration to avoid heavy GLFW include in the header.
struct GLFWwindow;

namespace Interstellar {
    namespace Input {
        namespace Keyboard {

            /**
             * @brief Concrete implementation of IKeyboardManager using GLFW.
             *
             * Contract:
             * - Call Update() once per frame after the window event pump has been serviced.
             * - Wire the GLFW key callback to OnGlfwKey(...) to feed transitions.
             * - Uses engine-level KeyCode for all queries; translation is injected.
             * - Edge queries (WasKeyPressed, WasKeyReleased) compare the committed
             *   state of the current frame against the previous frame.
             *
             * Thread-safety: not thread-safe. Callbacks and Update() must be invoked
             * on the same thread, typically the main thread that owns the GLFW window.
             *
             * @since 1.0
             */
            class KeyboardManager final : public IKeyboardManager {
            public:
                /**
                 * @brief Construct with a valid GLFW window and a keycode translator.
                 * @param window Non-owning pointer to a GLFWwindow. May be null in tests.
                 * @param translator Engine-to-platform key translator. Not owned and must
                 *        outlive this instance.
                 * @since 1.0
                 */
                explicit KeyboardManager(GLFWwindow* window, const IKeyCodeTranslator& translator) noexcept;

                ~KeyboardManager() noexcept override = default;

                KeyboardManager(const KeyboardManager&) = delete;
                KeyboardManager& operator=(const KeyboardManager&) = delete;
                KeyboardManager(KeyboardManager&&) = delete;
                KeyboardManager& operator=(KeyboardManager&&) = delete;

                /**
                 * @brief Advance frame state. Call once per frame after event polling.
                 *
                 * Publishes next-frame state into the current frame snapshot and rolls
                 * current into previous:
                 *   previous <- current
                 *   current  <- next
                 *
                 * Edge queries compare current vs previous.
                 *
                 * @since 1.0
                 */
                void Update() noexcept;

                // IKeyboardManager
                /**
                 * @copydoc IKeyboardManager::IsKeyDown
                 * @since 1.0
                 */
                [[nodiscard]] bool IsKeyDown(KeyCode key) const noexcept override;

                /**
                 * @copydoc IKeyboardManager::WasKeyPressed
                 * @since 1.0
                 */
                [[nodiscard]] bool WasKeyPressed(KeyCode key) const noexcept override;

                /**
                 * @copydoc IKeyboardManager::WasKeyReleased
                 * @since 1.0
                 */
                [[nodiscard]] bool WasKeyReleased(KeyCode key) const noexcept override;

                /**
                 * @brief Feed a GLFW key event into the manager.
                 *
                 * Intended to be called from the GLFW key callback:
                 *   void KeyCallback(GLFWwindow*, int key, int scancode, int action, int mods)
                 *   { keyboardManager.OnGlfwKey(key, scancode, action, mods); }
                 *
                 * Semantics:
                 * - GLFW_PRESS and GLFW_REPEAT mark the key as down in the "next" buffer.
                 * - GLFW_RELEASE marks the key as up in the "next" buffer.
                 * - Update() commits the "next" buffer, enabling edge detection.
                 *
                 * @param key GLFW key code (GLFW_KEY_*).
                 * @param scancode Platform scancode (unused).
                 * @param action GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT.
                 * @param mods Modifier bits from GLFW (unused).
                 * @since 1.0
                 */
                void OnGlfwKey(int key, int scancode, int action, int mods) noexcept;

                /**
                 * @brief Clear all key state on window focus loss.
                 *
                 * Clears previous, current, and next buffers to prevent stuck keys or
                 * spurious edges when the window regains focus.
                 *
                 * Intended to be called from the window focus callback when focus is lost.
                 *
                 * @since 1.0
                 */
                void OnFocusLost() noexcept;

            private:
                static constexpr std::size_t KeyCount = static_cast<std::size_t>(KeyCode::Count);
                using KeyArray = std::array<std::uint8_t, KeyCount>; // 0 or 1 per key

                GLFWwindow* m_Window;                   ///< Non-owning GLFW window handle.
                const IKeyCodeTranslator& m_Translator; ///< Non-owning keycode translator.

                KeyArray m_Current{};  ///< Committed state for this frame (1 if down).
                KeyArray m_Previous{}; ///< Committed state for the previous frame.
                KeyArray m_Next{};     ///< Event writes accumulate here until Update().

                /**
                 * @brief Translate a GLFW key to an engine KeyCode.
                 * @param glfwKey Source GLFW key code.
                 * @return Engine KeyCode value or KeyCode::Unknown if unmapped.
                 * @since 1.0
                 */
                [[nodiscard]] KeyCode TranslateFromGlfw(int glfwKey) const noexcept;
            };

        }
    }
} // namespace Interstellar::Input::Keyboard
