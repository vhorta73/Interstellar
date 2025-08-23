#pragma once
#include <Interstellar/Input/KeyCodes.hpp>

struct GLFWwindow; // fwd
namespace Interstellar::Input::Platforms::GLFW {

    KeyCode TranslateGLFWKey(int glfwKey);
    MouseButton TranslateGLFWMouseButton(int glfwButton);

} // namespace Interstellar::Input::Platforms::GLFW
