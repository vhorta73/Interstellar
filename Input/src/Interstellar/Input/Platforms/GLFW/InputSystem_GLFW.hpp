// InputSystem_GLFW.hpp
#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace Interstellar::Input::Platforms::GLFW {

    // Internal: InputSystem implementation for GLFW backend.
    // Installed as the active InputSystem when created via InputSystem::Create().
    class InputSystem_GLFW final : public Core::InputSystem_Impl {
    public:
        explicit InputSystem_GLFW(const InputConfig& cfg);
        ~InputSystem_GLFW() override;

        InputConfig::Backend backend() const override { return InputConfig::Backend::GLFW; }

        // Process OS events (delegates to glfwPollEvents()).
        void pump() override;

        // Commit staged device state at start of frame.
        void beginFrame(double dt) override;

        // Nothing to finalize for GLFW backend.
        void endFrame() override {}

    private:
        // GLFW callback handlers.
        static void KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* w, int button, int action, int mods);
        static void CursorPosCallback(GLFWwindow* w, double x, double y);
        static void ScrollCallback(GLFWwindow* w, double xoff, double yoff);

        // Lookup helper: find backend instance from GLFWwindow*.
        static InputSystem_GLFW* From(GLFWwindow* w);

        GLFWwindow* window_{ nullptr };

        // Preserve any previously installed GLFW callbacks and chain to them.
        GLFWkeyfun         prevKeyCb_{ nullptr };
        GLFWmousebuttonfun prevMouseButtonCb_{ nullptr };
        GLFWcursorposfun   prevCursorPosCb_{ nullptr };
        GLFWscrollfun      prevScrollCb_{ nullptr };

        // Global registry mapping GLFWwindow* to InputSystem_GLFW*.
        static std::unordered_map<GLFWwindow*, InputSystem_GLFW*>& Reg();
    };

} // namespace Interstellar::Input::Platforms::GLFW
