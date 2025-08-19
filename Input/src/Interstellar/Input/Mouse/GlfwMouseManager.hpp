#pragma once
/**
 * @file
 * @brief GLFW-backed mouse manager implementing IMouseManager.
 * @ingroup PlatformImplementations
 * @ingroup InputSystem
 * @since 1.0
 */

#include "Interstellar/Input/IMouseManager.hpp"
#include "Interstellar/Input/MouseButton.hpp"

 // Do not include <GLFW/glfw3.h> here; keep the header light.
struct GLFWwindow;

namespace Interstellar {
    namespace Input {
        namespace Mouse {

            /**
             * @brief Concrete mouse input handler for GLFW-based systems.
             *
             * Contract:
             * - Coordinates are window-space pixels with origin at top-left (Y increases downward).
             * - Call Update() once per frame after the window event pump has been serviced.
             * - Wire GLFW callbacks to the OnGlfw* methods to feed live input.
             * - Edge queries (WasButtonPressed/WasButtonReleased) compare the committed
             *   state of the current frame against the previous frame.
             *
             * Semantics:
             * - Button events (PRESS/RELEASE) are written into a "next" buffer via OnGlfwMouseButton().
             * - Update() commits "next" into the current frame and rolls current into previous:
             *     previous <- current
             *     current  <- next
             * - Scroll offsets accumulate until ResetScrollOffsets() is called.
             * - Dragging is reported when the left button is down and movement exceeds a small threshold
             *   between consecutive frames.
             *
             * Thread-safety:
             * - Not thread-safe. Callbacks and Update() must be invoked on the same thread, typically
             *   the main thread that owns the GLFW window.
             *
             * @since 1.0
             */
            class GlfwMouseManager final : public IMouseManager {
            public:
                /**
                 * @brief Maximum number of mouse buttons tracked.
                 * Exposed for compile-time checks in tests and translation helpers.
                 * @since 1.0
                 */
                static constexpr int ButtonCountPublic = 8;

                /**
                 * @brief Returns the button capacity tracked by this manager.
                 * @since 1.0
                 */
                static constexpr int ButtonCapacity() noexcept { return ButtonCountPublic; }

                /**
                 * @brief Construct the manager for a given GLFW window.
                 * @param window Non-owning pointer to a GLFWwindow. May be null in tests.
                 * @since 1.0
                 */
                explicit GlfwMouseManager(GLFWwindow* window) noexcept;

                ~GlfwMouseManager() noexcept override = default;

                GlfwMouseManager(const GlfwMouseManager&) = delete;
                GlfwMouseManager& operator=(const GlfwMouseManager&) = delete;
                GlfwMouseManager(GlfwMouseManager&&) = delete;
                GlfwMouseManager& operator=(GlfwMouseManager&&) = delete;

                /**
                 * @brief Advance frame state. Call once per frame after event polling.
                 *
                 * Publishes the "next" button state into the current frame snapshot and rolls
                 * current into previous. Dragging is computed from the committed positions.
                 *
                 * @since 1.0
                 */
                void Update() noexcept;

                // IMouseManager ----------------------------------------------------------

                /**
                 * @copydoc IMouseManager::GetX
                 * @since 1.0
                 */
                [[nodiscard]] double GetX() const noexcept override;

                /**
                 * @copydoc IMouseManager::GetY
                 * @since 1.0
                 */
                [[nodiscard]] double GetY() const noexcept override;

                /**
                 * @copydoc IMouseManager::IsButtonDown
                 * @since 1.0
                 */
                [[nodiscard]] bool IsButtonDown(MouseButton button) const noexcept override;

                /**
                 * @copydoc IMouseManager::WasButtonPressed
                 * @since 1.0
                 */
                [[nodiscard]] bool WasButtonPressed(MouseButton button) const noexcept override;

                /**
                 * @copydoc IMouseManager::WasButtonReleased
                 * @since 1.0
                 */
                [[nodiscard]] bool WasButtonReleased(MouseButton button) const noexcept override;

                /**
                 * @copydoc IMouseManager::GetScrollOffsetY
                 * @since 1.0
                 */
                [[nodiscard]] double GetScrollOffsetY() const noexcept override;

                /**
                 * @copydoc IMouseManager::GetScrollOffsetX
                 * @since 1.0
                 */
                [[nodiscard]] double GetScrollOffsetX() const noexcept override;

                /**
                 * @copydoc IMouseManager::IsDragging
                 * @since 1.0
                 */
                [[nodiscard]] bool IsDragging() const noexcept override;

                // GLFW event entry points -----------------------------------------------

                /**
                 * @brief Feed a GLFW cursor position event.
                 * Coordinates must be window-space pixels (origin top-left).
                 * @since 1.0
                 */
                void OnGlfwCursorPos(double x, double y) noexcept;

                /**
                 * @brief Feed a GLFW mouse button event.
                 *
                 * Semantics:
                 * - GLFW_PRESS and GLFW_REPEAT mark the translated button as down in the "next" buffer.
                 * - GLFW_RELEASE marks it up in the "next" buffer.
                 * - Update() commits these changes, enabling edge detection.
                 *
                 * @param button GLFW mouse button code (GLFW_MOUSE_BUTTON_*).
                 * @param action GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT.
                 * @param mods   GLFW modifier bits (unused here).
                 * @since 1.0
                 */
                void OnGlfwMouseButton(int button, int action, int mods) noexcept;

                /**
                 * @brief Feed a GLFW scroll event. Offsets accumulate until reset.
                 * @param xoffset Horizontal scroll delta.
                 * @param yoffset Vertical scroll delta.
                 * @since 1.0
                 */
                void OnGlfwScroll(double xoffset, double yoffset) noexcept;

                /**
                 * @brief Clear all mouse state on window focus loss.
                 *
                 * Resets button buffers, drag flag, and scroll accumulators to prevent
                 * stuck states or spurious edges when the window regains focus.
                 *
                 * @since 1.0
                 */
                void OnGlfwFocusLost() noexcept;

                /**
                 * @brief Reset accumulated scroll offsets to zero.
                 * Call after consuming scroll for the frame.
                 * @since 1.0
                 */
                void ResetScrollOffsets() noexcept;

            private:
                GLFWwindow* m_Window; // not owned

                // Positions
                double m_X = 0.0;
                double m_Y = 0.0;
                double m_LastX = 0.0;
                double m_LastY = 0.0;

                // Button states: 1 if down, 0 if up
                unsigned char m_CurrButtons[ButtonCountPublic] = { 0 };
                unsigned char m_PrevButtons[ButtonCountPublic] = { 0 };
                unsigned char m_NextButtons[ButtonCountPublic] = { 0 };

                // Scroll accumulation since last reset
                double m_ScrollX = 0.0;
                double m_ScrollY = 0.0;

                bool m_IsDragging = false;

                // Translation helpers
                static int ToIndex(MouseButton b) noexcept;
                static MouseButton FromGlfwButton(int glfwButton) noexcept;
            };

        }
    }
} // namespace Interstellar::Input::Mouse
