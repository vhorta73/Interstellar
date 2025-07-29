#include "KeyboardManager.hpp"
#include <GLFW/glfw3.h>

namespace Interstellar::Input {

    KeyboardManager::KeyboardManager(GLFWwindow* window)
        : m_Window(window) {
    }

    void KeyboardManager::Update() {
        if (!m_Window) return;

        m_PreviousState = m_CurrentState;

        // Poll all keys up to GLFW_KEY_LAST
        for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
            int state = glfwGetKey(m_Window, key);
            m_CurrentState[key] = (state == GLFW_PRESS || state == GLFW_REPEAT);
        }
    }

    bool KeyboardManager::IsKeyDown(int key) const {
        auto it = m_CurrentState.find(key);
        return it != m_CurrentState.end() && it->second;
    }

    bool KeyboardManager::WasKeyPressed(int key) const {
        const auto curr = m_CurrentState.find(key);
        const auto prev = m_PreviousState.find(key);
        return (curr != m_CurrentState.end() && curr->second) &&
            (prev == m_PreviousState.end() || !prev->second);
    }

    bool KeyboardManager::WasKeyReleased(int key) const {
        const auto curr = m_CurrentState.find(key);
        const auto prev = m_PreviousState.find(key);
        return (prev != m_PreviousState.end() && prev->second) &&
            (curr == m_CurrentState.end() || !curr->second);
    }
}
