// InputSystem_GLFW.cpp (changes)
#include "InputSystem_GLFW.hpp"
#include "KeyTranslation_GLFW.hpp"
#include <cassert>

namespace Interstellar::Input::Platforms::GLFW {

    static std::unordered_map<GLFWwindow*, InputSystem_GLFW*> g_registry;

    std::unordered_map<GLFWwindow*, InputSystem_GLFW*>& InputSystem_GLFW::Reg() { return g_registry; }
    InputSystem_GLFW* InputSystem_GLFW::From(GLFWwindow* w) {
        auto it = Reg().find(w);
        return it == Reg().end() ? nullptr : it->second;
    }

    InputSystem_GLFW::InputSystem_GLFW(const InputConfig& cfg) {
        window_ = static_cast<GLFWwindow*>(cfg.nativeWindow);
        assert(window_ && "GLFW backend requires a valid GLFWwindow*");

        // Register ourselves (no user-pointer reliance)
        Reg()[window_] = this;

        // Chain: grab existing callbacks, then install ours
        prevKeyCb_ = glfwSetKeyCallback(window_, &InputSystem_GLFW::KeyCallback);
        prevMouseButtonCb_ = glfwSetMouseButtonCallback(window_, &InputSystem_GLFW::MouseButtonCallback);
        prevCursorPosCb_ = glfwSetCursorPosCallback(window_, &InputSystem_GLFW::CursorPosCallback);
        prevScrollCb_ = glfwSetScrollCallback(window_, &InputSystem_GLFW::ScrollCallback);

        // Initialize mouse pos
        double x = 0, y = 0; glfwGetCursorPos(window_, &x, &y);
        mouse_.setPosition(static_cast<float>(x), static_cast<float>(y));
        mouse_.beginFrame(0.0);
    }

    InputSystem_GLFW::~InputSystem_GLFW() {
        // Restore previous callbacks (be a good citizen)
        if (window_) {
            glfwSetKeyCallback(window_, prevKeyCb_);
            glfwSetMouseButtonCallback(window_, prevMouseButtonCb_);
            glfwSetCursorPosCallback(window_, prevCursorPosCb_);
            glfwSetScrollCallback(window_, prevScrollCb_);
            Reg().erase(window_);
        }
    }

    void InputSystem_GLFW::pump() {
        // ok to call even if app also calls it
        glfwPollEvents();
    }

    void InputSystem_GLFW::beginFrame(double dt) {
        keyboard_.beginFrame(dt);
        mouse_.beginFrame(dt);
    }

    /* static */ void InputSystem_GLFW::KeyCallback(GLFWwindow* w, int key, int sc, int action, int mods) {
        if (auto* self = From(w)) {
            const auto kc = TranslateGLFWKey(key);
            if (kc != KeyCode::Unknown) {
                const bool down = (action != GLFW_RELEASE);
                self->keyboard_.setKeyDown(kc, down);
            }
            if (self->prevKeyCb_) self->prevKeyCb_(w, key, sc, action, mods);
        }
        else {
            // Not our window; just pass through if someone chained us
            // (we have no prev here, nothing to do)
        }
    }

    /* static */ void InputSystem_GLFW::MouseButtonCallback(GLFWwindow* w, int button, int action, int mods) {
        if (auto* self = From(w)) {
            const auto mb = TranslateGLFWMouseButton(button);
            const bool down = (action != GLFW_RELEASE);
            self->mouse_.setButton(mb, down);
            if (self->prevMouseButtonCb_) self->prevMouseButtonCb_(w, button, action, mods);
        }
    }

    /* static */ void InputSystem_GLFW::CursorPosCallback(GLFWwindow* w, double x, double y) {
        if (auto* self = From(w)) {
            self->mouse_.setPosition(static_cast<float>(x), static_cast<float>(y));
            if (self->prevCursorPosCb_) self->prevCursorPosCb_(w, x, y);
        }
    }

    void InputSystem_GLFW::ScrollCallback(GLFWwindow* w, double xoff, double yoff) {
        if (auto* self = From(w)) {
            self->mouse_.addWheel(static_cast<float>(xoff), static_cast<float>(yoff));
            if (self->prevScrollCb_) self->prevScrollCb_(w, xoff, yoff);
        }
    }


} // namespace Interstellar::Input::Platforms::GLFW
