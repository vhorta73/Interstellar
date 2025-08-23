#include "KeyTranslation_GLFW.hpp"
#include <GLFW/glfw3.h>

namespace Interstellar::Input::Platforms::GLFW {

    KeyCode TranslateGLFWKey(int k) {
        switch (k) {
        case GLFW_KEY_ESCAPE: return KeyCode::Escape;
        case GLFW_KEY_ENTER:  return KeyCode::Enter;
        case GLFW_KEY_TAB:    return KeyCode::Tab;
        case GLFW_KEY_BACKSPACE: return KeyCode::Backspace;
        case GLFW_KEY_SPACE:  return KeyCode::Space;

        case GLFW_KEY_LEFT:   return KeyCode::ArrowLeft;
        case GLFW_KEY_RIGHT:  return KeyCode::ArrowRight;
        case GLFW_KEY_UP:     return KeyCode::ArrowUp;
        case GLFW_KEY_DOWN:   return KeyCode::ArrowDown;

        case GLFW_KEY_0: return KeyCode::Num0;
        case GLFW_KEY_1: return KeyCode::Num1;
        case GLFW_KEY_2: return KeyCode::Num2;
        case GLFW_KEY_3: return KeyCode::Num3;
        case GLFW_KEY_4: return KeyCode::Num4;
        case GLFW_KEY_5: return KeyCode::Num5;
        case GLFW_KEY_6: return KeyCode::Num6;
        case GLFW_KEY_7: return KeyCode::Num7;
        case GLFW_KEY_8: return KeyCode::Num8;
        case GLFW_KEY_9: return KeyCode::Num9;

        case GLFW_KEY_A: return KeyCode::A;
        case GLFW_KEY_B: return KeyCode::B;
        case GLFW_KEY_C: return KeyCode::C;
        case GLFW_KEY_D: return KeyCode::D;
        case GLFW_KEY_E: return KeyCode::E;
        case GLFW_KEY_F: return KeyCode::F;
        case GLFW_KEY_G: return KeyCode::G;
        case GLFW_KEY_H: return KeyCode::H;
        case GLFW_KEY_I: return KeyCode::I;
        case GLFW_KEY_J: return KeyCode::J;
        case GLFW_KEY_K: return KeyCode::K;
        case GLFW_KEY_L: return KeyCode::L;
        case GLFW_KEY_M: return KeyCode::M;
        case GLFW_KEY_N: return KeyCode::N;
        case GLFW_KEY_O: return KeyCode::O;
        case GLFW_KEY_P: return KeyCode::P;
        case GLFW_KEY_Q: return KeyCode::Q;
        case GLFW_KEY_R: return KeyCode::R;
        case GLFW_KEY_S: return KeyCode::S;
        case GLFW_KEY_T: return KeyCode::T;
        case GLFW_KEY_U: return KeyCode::U;
        case GLFW_KEY_V: return KeyCode::V;
        case GLFW_KEY_W: return KeyCode::W;
        case GLFW_KEY_X: return KeyCode::X;
        case GLFW_KEY_Y: return KeyCode::Y;
        case GLFW_KEY_Z: return KeyCode::Z;

        case GLFW_KEY_F1:  return KeyCode::F1;
        case GLFW_KEY_F2:  return KeyCode::F2;
        case GLFW_KEY_F3:  return KeyCode::F3;
        case GLFW_KEY_F4:  return KeyCode::F4;
        case GLFW_KEY_F5:  return KeyCode::F5;
        case GLFW_KEY_F6:  return KeyCode::F6;
        case GLFW_KEY_F7:  return KeyCode::F7;
        case GLFW_KEY_F8:  return KeyCode::F8;
        case GLFW_KEY_F9:  return KeyCode::F9;
        case GLFW_KEY_F10: return KeyCode::F10;
        case GLFW_KEY_F11: return KeyCode::F11;
        case GLFW_KEY_F12: return KeyCode::F12;
        default: return KeyCode::Unknown;
        }
    }

    MouseButton TranslateGLFWMouseButton(int b) {
        switch (b) {
        case GLFW_MOUSE_BUTTON_LEFT:   return MouseButton::Left;
        case GLFW_MOUSE_BUTTON_RIGHT:  return MouseButton::Right;
        case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
        default: return MouseButton::Left; // fallback; extend if needed
        }
    }

} // namespace Interstellar::Input::Platforms::GLFW
