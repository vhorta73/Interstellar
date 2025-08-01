#include "Input/Mouse/GlfwMouseManager.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

namespace Interstellar::Input::Mouse {

   
    GlfwMouseManager::GlfwMouseManager(GLFWwindow* window)
        : m_Window(window) {
        if (m_Window) {
            glfwSetWindowUserPointer(m_Window, this);
            glfwSetScrollCallback(m_Window, ScrollCallback);
        }
    }

    void GlfwMouseManager::ScrollCallback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
        auto* self = static_cast<GlfwMouseManager*>(glfwGetWindowUserPointer(window));
        if (self) {
            self->m_ScrollOffsetY += yoffset;
        }
    }

    void GlfwMouseManager::Update() {
        if (!m_Window) return;

        // Store previous position before updating
        m_LastX = m_X;
        m_LastY = m_Y;

        // Query cursor position from GLFW
        glfwGetCursorPos(m_Window, &m_X, &m_Y);

        // Check if the left mouse button is pressed
        bool leftPressed = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        constexpr double kDragThreshold = 1.5; // Pixels, squared distance
        double dx = m_X - m_LastX;
        double dy = m_Y - m_LastY;

        m_IsDragging = leftPressed && (dx * dx + dy * dy > kDragThreshold * kDragThreshold);

        //if (m_IsDragging)
            //std::cout << "Dragging started!" << std::endl;

    }
    void GlfwMouseManager::ResetScrollOffset() {
        m_ScrollOffsetY = 0.0;
    }
    double GlfwMouseManager::GetScrollOffsetY() const {
        return m_ScrollOffsetY;
    }

    double GlfwMouseManager::GetX() const {
        return m_X;
    }

    double GlfwMouseManager::GetY() const {
        return m_Y;
    }

    bool GlfwMouseManager::IsButtonPressed(int button) const {
        if (!m_Window) return false;
        return glfwGetMouseButton(m_Window, button) == GLFW_PRESS;
    }

    bool GlfwMouseManager::IsDragging() const {
        return m_IsDragging;
    }

    //GlfwMouseManager::GlfwMouseManager(GLFWwindow* window)
        //: m_Window(window) {
        //if (m_Window) {
            //glfwSetWindowUserPointer(m_Window, this);
            //glfwSetScrollCallback(m_Window, [](GLFWwindow* w, double /*x*/, double y) {
                //auto* self = static_cast<GlfwMouseManager*>(glfwGetWindowUserPointer(w));
                //if (self) {
                    //self->m_ScrollOffsetY += y;
                //}
                //});
        //}
    //}


    //double GlfwMouseManager::GetScrollOffsetY() {
        //double value = m_ScrollOffsetY;
        //m_ScrollOffsetY = 0.0;
        //return value;
    //}

}
