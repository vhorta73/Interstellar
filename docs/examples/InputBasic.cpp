// docs/examples/InputBasic.cpp
#include <Interstellar/Input/IInputManager.hpp>
#include <Interstellar/Input/KeyCode.hpp>
#include <Interstellar/Input/MouseButton.hpp>

// Forward-declare GLFW types to keep this snippet standalone for docs.
// In a real build, include <GLFW/glfw3.h> and link GLFW.
struct GLFWwindow;

void GameLoop(GLFWwindow* window)
{
    auto input = Interstellar::Input::IInputManager::Create(window);

    for (;;) {
        // glfwPollEvents(); // in real code
        input->Update();

        auto& kb = input->GetKeyboardManager();
        if (kb.WasKeyPressed(Interstellar::Input::KeyCode::Space)) {
            // Jump, confirm, etc.
        }

        auto& mouse = input->GetMouseManager();
        if (mouse.WasButtonReleased(Interstellar::Input::MouseButton::Left)) {
            // Handle click release.
        }

        // ... game logic ...
        break; // example only
    }
}
