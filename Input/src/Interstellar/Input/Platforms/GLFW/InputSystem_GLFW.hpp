// InputSystem_GLFW.hpp (changes)
#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>
#include "../../Core/InputSystem_Impl.hpp"

namespace Interstellar::Input::Platforms::GLFW {

    class InputSystem_GLFW final : public Core::InputSystem_Impl {
    public:
        explicit InputSystem_GLFW(const InputConfig& cfg);
        ~InputSystem_GLFW() override;

        InputConfig::Backend backend() const override { return InputConfig::Backend::GLFW; }
        void pump() override;
        void beginFrame(double dt) override;
        void endFrame() override {}

    private:
        // Our handlers
        static void KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* w, int button, int action, int mods);
        static void CursorPosCallback(GLFWwindow* w, double x, double y);
        static void ScrollCallback(GLFWwindow* w, double xoff, double yoff);

        static InputSystem_GLFW* From(GLFWwindow* w);

        GLFWwindow* window_{ nullptr };

        // Chain to previous callbacks if they existed
        GLFWkeyfun            prevKeyCb_{ nullptr };
        GLFWmousebuttonfun    prevMouseButtonCb_{ nullptr };
        GLFWcursorposfun      prevCursorPosCb_{ nullptr };
        GLFWscrollfun         prevScrollCb_{ nullptr };

        // Registry of window -> backend
        static std::unordered_map<GLFWwindow*, InputSystem_GLFW*>& Reg();
    };

} // namespace Interstellar::Input::Platforms::GLFW
