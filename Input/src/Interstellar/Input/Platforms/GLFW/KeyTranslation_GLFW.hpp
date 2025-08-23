#pragma once
#include <Interstellar/Input/KeyCodes.hpp>

struct GLFWwindow; // forward declaration

namespace Interstellar::Input::Platforms::GLFW {

    // Translate a GLFW key code into the engine's unified KeyCode.
    KeyCode TranslateGLFWKey(int glfwKey);

    // Translate a GLFW mouse button into the engine's unified MouseButton.
    MouseButton TranslateGLFWMouseButton(int glfwButton);

} // namespace Interstellar::Input::Platforms::GLFW
