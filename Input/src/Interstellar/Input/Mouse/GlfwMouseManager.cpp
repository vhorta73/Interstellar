// File: src/Interstellar/Input/Mouse/GlfwMouseManager.cpp

#include "Interstellar/Input/Mouse/GlfwMouseManager.hpp"

// GLFW stays in the .cpp
#include <GLFW/glfw3.h>
#include <cstring> // for std::memcpy

namespace Interstellar {
    namespace Input {
        namespace Mouse {

            // Compile-time sanity: we expect at least 8 buttons available.
            static_assert(GlfwMouseManager::ButtonCapacity() >= 8, "GlfwMouseManager::ButtonCapacity must be >= 8");

            int GlfwMouseManager::ToIndex(MouseButton b) noexcept {
                switch (b) {
                case MouseButton::Left:    return 0;
                case MouseButton::Right:   return 1;
                case MouseButton::Middle:  return 2;
                case MouseButton::Button4: return 3;
                case MouseButton::Button5: return 4;
                case MouseButton::Button6: return 5;
                case MouseButton::Button7: return 6;
                case MouseButton::Button8: return 7;
                default:                   return -1;
                }
            }

            MouseButton GlfwMouseManager::FromGlfwButton(int glfwButton) noexcept {
                switch (glfwButton) {
                case GLFW_MOUSE_BUTTON_LEFT:   return MouseButton::Left;
                case GLFW_MOUSE_BUTTON_RIGHT:  return MouseButton::Right;
                case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
                case GLFW_MOUSE_BUTTON_4:      return MouseButton::Button4;
                case GLFW_MOUSE_BUTTON_5:      return MouseButton::Button5;
                case GLFW_MOUSE_BUTTON_6:      return MouseButton::Button6;
                case GLFW_MOUSE_BUTTON_7:      return MouseButton::Button7;
                case GLFW_MOUSE_BUTTON_8:      return MouseButton::Button8;
                default:                       return MouseButton::Unknown;
                }
            }

            GlfwMouseManager::GlfwMouseManager(GLFWwindow* window) noexcept
                : m_Window(window)
            {
                // Do not install GLFW callbacks here. Wire callbacks externally and
                // forward events to OnGlfwCursorPos / OnGlfwMouseButton / OnGlfwScroll.
            }

            void GlfwMouseManager::Update() noexcept
            {
                // Publish next -> current and advance edges
                std::memcpy(m_PrevButtons, m_CurrButtons, sizeof(m_CurrButtons));
                std::memcpy(m_CurrButtons, m_NextButtons, sizeof(m_CurrButtons));

                // Drag policy: left button held and movement above threshold since last frame.
                const double dx = m_X - m_LastX;
                const double dy = m_Y - m_LastY;
                static constexpr double kDragThresholdPixels = 1.5;
                static constexpr double kDragThresholdSq = kDragThresholdPixels * kDragThresholdPixels;

                const int leftIdx = ToIndex(MouseButton::Left);
                const bool leftDown = (leftIdx >= 0) ? (m_CurrButtons[leftIdx] != 0) : false;
                m_IsDragging = leftDown && ((dx * dx + dy * dy) > kDragThresholdSq);

                // Prepare for next frame's delta
                m_LastX = m_X;
                m_LastY = m_Y;

                // Note: do NOT clear m_NextButtons here; callbacks maintain it.
            }

            double GlfwMouseManager::GetX() const noexcept
            {
                return m_X;
            }

            double GlfwMouseManager::GetY() const noexcept
            {
                return m_Y;
            }

            bool GlfwMouseManager::IsButtonDown(MouseButton button) const noexcept
            {
                const int idx = ToIndex(button);
                if (idx < 0 || idx >= GlfwMouseManager::ButtonCapacity()) return false;
                return m_CurrButtons[idx] != 0;
            }

            bool GlfwMouseManager::WasButtonPressed(MouseButton button) const noexcept
            {
                const int idx = ToIndex(button);
                if (idx < 0 || idx >= GlfwMouseManager::ButtonCapacity()) return false;
                const bool curr = m_CurrButtons[idx] != 0;
                const bool prev = m_PrevButtons[idx] != 0;
                return curr && !prev;
            }

            bool GlfwMouseManager::WasButtonReleased(MouseButton button) const noexcept
            {
                const int idx = ToIndex(button);
                if (idx < 0 || idx >= GlfwMouseManager::ButtonCapacity()) return false;
                const bool curr = m_CurrButtons[idx] != 0;
                const bool prev = m_PrevButtons[idx] != 0;
                return !curr && prev;
            }

            double GlfwMouseManager::GetScrollOffsetY() const noexcept
            {
                return m_ScrollY;
            }

            double GlfwMouseManager::GetScrollOffsetX() const noexcept
            {
                return m_ScrollX;
            }

            bool GlfwMouseManager::IsDragging() const noexcept
            {
                return m_IsDragging;
            }

            void GlfwMouseManager::OnGlfwCursorPos(double x, double y) noexcept
            {
                // Window-space pixels, origin top-left
                m_X = x;
                m_Y = y;
            }

            void GlfwMouseManager::OnGlfwMouseButton(int button, int action, int /*mods*/) noexcept
            {
                const MouseButton mb = FromGlfwButton(button);
                const int idx = ToIndex(mb);
                if (idx < 0 || idx >= GlfwMouseManager::ButtonCapacity()) return;

                switch (action)
                {
                case GLFW_PRESS:   // GLFW doesn't emit repeats for mouse; PRESS means down
                case GLFW_REPEAT:  // defensive
                    m_NextButtons[idx] = 1u;
                    break;
                case GLFW_RELEASE:
                    m_NextButtons[idx] = 0u;
                    break;
                default:
                    break;
                }
            }

            void GlfwMouseManager::OnGlfwScroll(double xoffset, double yoffset) noexcept
            {
                m_ScrollX += xoffset;
                m_ScrollY += yoffset;
            }

            void GlfwMouseManager::ResetScrollOffsets() noexcept
            {
                m_ScrollX = 0.0;
                m_ScrollY = 0.0;
            }

            void GlfwMouseManager::OnGlfwFocusLost() noexcept
            {
                for (int i = 0; i < ButtonCapacity(); ++i) {
                    m_NextButtons[i] = 0u;
                    m_CurrButtons[i] = 0u;
                    m_PrevButtons[i] = 0u;
                }
                m_IsDragging = false;
                m_ScrollX = 0.0;
                m_ScrollY = 0.0;
            }

        }
    }
} // namespace Interstellar::Input::Mouse
