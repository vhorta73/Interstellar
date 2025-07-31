#include "MouseManager.hpp"
#include <GLFW/glfw3.h>

namespace Interstellar::Input {

    MouseManager::MouseManager(GLFWwindow* window)
        : m_Window(window) {
    }

    void MouseManager::Update() {
        if (!m_Window) return;

        // Query cursor position from GLFW
        glfwGetCursorPos(m_Window, &m_X, &m_Y);

        // Check if the left mouse button is pressed
        bool leftPressed = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        m_IsDragging = leftPressed && (m_X != m_LastX || m_Y != m_LastY);
    }

    double MouseManager::GetX() const {
        return m_X;
    }

    double MouseManager::GetY() const {
        return m_Y;
    }

    bool MouseManager::IsButtonPressed(int button) const {
        if (!m_Window) return false;
        return glfwGetMouseButton(m_Window, button) == GLFW_PRESS;
    }

    bool MouseManager::IsDragging() const {
        return m_IsDragging;
    }
}
