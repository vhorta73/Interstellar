#include "Interstellar/Input/KeyCodeTranslator/GlfwKeyMap.hpp"

#include <GLFW/glfw3.h>

namespace Interstellar {
    namespace Input {
        namespace KeyCodeTranslator {

            using Interstellar::Input::KeyCode;

            KeyCode GLFWKeyMap::FromPlatform(int key) const noexcept
            {
                switch (key)
                {
                    // Alphabet
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

                    // Number row
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

                    // Function keys
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

                    // Extended function keys (if produced by platform)
                case GLFW_KEY_F13: return KeyCode::F13;
                case GLFW_KEY_F14: return KeyCode::F14;
                case GLFW_KEY_F15: return KeyCode::F15;
                case GLFW_KEY_F16: return KeyCode::F16;
                case GLFW_KEY_F17: return KeyCode::F17;
                case GLFW_KEY_F18: return KeyCode::F18;
                case GLFW_KEY_F19: return KeyCode::F19;
                case GLFW_KEY_F20: return KeyCode::F20;
                case GLFW_KEY_F21: return KeyCode::F21;
                case GLFW_KEY_F22: return KeyCode::F22;
                case GLFW_KEY_F23: return KeyCode::F23;
                case GLFW_KEY_F24: return KeyCode::F24;

                    // Modifiers
                case GLFW_KEY_LEFT_SHIFT:   return KeyCode::LeftShift;
                case GLFW_KEY_RIGHT_SHIFT:  return KeyCode::RightShift;
                case GLFW_KEY_LEFT_CONTROL: return KeyCode::LeftControl;
                case GLFW_KEY_RIGHT_CONTROL:return KeyCode::RightControl;
                case GLFW_KEY_LEFT_ALT:     return KeyCode::LeftAlt;
                case GLFW_KEY_RIGHT_ALT:    return KeyCode::RightAlt;
                case GLFW_KEY_LEFT_SUPER:   return KeyCode::LeftSuper;
                case GLFW_KEY_RIGHT_SUPER:  return KeyCode::RightSuper;

                    // Arrows
                case GLFW_KEY_UP:    return KeyCode::ArrowUp;
                case GLFW_KEY_DOWN:  return KeyCode::ArrowDown;
                case GLFW_KEY_LEFT:  return KeyCode::ArrowLeft;
                case GLFW_KEY_RIGHT: return KeyCode::ArrowRight;

                    // Navigation and editing
                case GLFW_KEY_HOME:     return KeyCode::Home;
                case GLFW_KEY_END:      return KeyCode::End;
                case GLFW_KEY_PAGE_UP:  return KeyCode::PageUp;
                case GLFW_KEY_PAGE_DOWN:return KeyCode::PageDown;
                case GLFW_KEY_INSERT:   return KeyCode::Insert;
                case GLFW_KEY_DELETE:   return KeyCode::Delete;

                    // Control
                case GLFW_KEY_ESCAPE:   return KeyCode::Escape;
                case GLFW_KEY_TAB:      return KeyCode::Tab;
                case GLFW_KEY_ENTER:    return KeyCode::Enter;
                case GLFW_KEY_BACKSPACE:return KeyCode::Backspace;
                case GLFW_KEY_SPACE:    return KeyCode::Space;

                    // Locks
                case GLFW_KEY_CAPS_LOCK:   return KeyCode::CapsLock;
                case GLFW_KEY_SCROLL_LOCK: return KeyCode::ScrollLock;
                case GLFW_KEY_NUM_LOCK:    return KeyCode::NumLock;

                    // Punctuation (US)
                case GLFW_KEY_GRAVE_ACCENT: return KeyCode::Grave;
                case GLFW_KEY_MINUS:        return KeyCode::Minus;
                case GLFW_KEY_EQUAL:        return KeyCode::Equal;
                case GLFW_KEY_LEFT_BRACKET: return KeyCode::LeftBracket;
                case GLFW_KEY_RIGHT_BRACKET:return KeyCode::RightBracket;
                case GLFW_KEY_BACKSLASH:    return KeyCode::Backslash;
                case GLFW_KEY_SEMICOLON:    return KeyCode::Semicolon;
                case GLFW_KEY_APOSTROPHE:   return KeyCode::Apostrophe;
                case GLFW_KEY_COMMA:        return KeyCode::Comma;
                case GLFW_KEY_PERIOD:       return KeyCode::Period;
                case GLFW_KEY_SLASH:        return KeyCode::Slash;

                    // Special/system
                case GLFW_KEY_PRINT_SCREEN: return KeyCode::PrintScreen;
                case GLFW_KEY_PAUSE:        return KeyCode::Pause;
                case GLFW_KEY_MENU:         return KeyCode::Application;

                    // Numpad
                case GLFW_KEY_KP_0:        return KeyCode::Kp0;
                case GLFW_KEY_KP_1:        return KeyCode::Kp1;
                case GLFW_KEY_KP_2:        return KeyCode::Kp2;
                case GLFW_KEY_KP_3:        return KeyCode::Kp3;
                case GLFW_KEY_KP_4:        return KeyCode::Kp4;
                case GLFW_KEY_KP_5:        return KeyCode::Kp5;
                case GLFW_KEY_KP_6:        return KeyCode::Kp6;
                case GLFW_KEY_KP_7:        return KeyCode::Kp7;
                case GLFW_KEY_KP_8:        return KeyCode::Kp8;
                case GLFW_KEY_KP_9:        return KeyCode::Kp9;
                case GLFW_KEY_KP_DECIMAL:  return KeyCode::KpDecimal;
                case GLFW_KEY_KP_DIVIDE:   return KeyCode::KpDivide;
                case GLFW_KEY_KP_MULTIPLY: return KeyCode::KpMultiply;
                case GLFW_KEY_KP_SUBTRACT: return KeyCode::KpSubtract;
                case GLFW_KEY_KP_ADD:      return KeyCode::KpAdd;
                case GLFW_KEY_KP_ENTER:    return KeyCode::KpEnter;
                case GLFW_KEY_KP_EQUAL:    return KeyCode::KpEqual;

                default:
                    return KeyCode::Unknown;
                }
            }

            int GLFWKeyMap::ToPlatform(KeyCode code) const noexcept
            {
                switch (code)
                {
                    // Alphabet
                case KeyCode::A: return GLFW_KEY_A;
                case KeyCode::B: return GLFW_KEY_B;
                case KeyCode::C: return GLFW_KEY_C;
                case KeyCode::D: return GLFW_KEY_D;
                case KeyCode::E: return GLFW_KEY_E;
                case KeyCode::F: return GLFW_KEY_F;
                case KeyCode::G: return GLFW_KEY_G;
                case KeyCode::H: return GLFW_KEY_H;
                case KeyCode::I: return GLFW_KEY_I;
                case KeyCode::J: return GLFW_KEY_J;
                case KeyCode::K: return GLFW_KEY_K;
                case KeyCode::L: return GLFW_KEY_L;
                case KeyCode::M: return GLFW_KEY_M;
                case KeyCode::N: return GLFW_KEY_N;
                case KeyCode::O: return GLFW_KEY_O;
                case KeyCode::P: return GLFW_KEY_P;
                case KeyCode::Q: return GLFW_KEY_Q;
                case KeyCode::R: return GLFW_KEY_R;
                case KeyCode::S: return GLFW_KEY_S;
                case KeyCode::T: return GLFW_KEY_T;
                case KeyCode::U: return GLFW_KEY_U;
                case KeyCode::V: return GLFW_KEY_V;
                case KeyCode::W: return GLFW_KEY_W;
                case KeyCode::X: return GLFW_KEY_X;
                case KeyCode::Y: return GLFW_KEY_Y;
                case KeyCode::Z: return GLFW_KEY_Z;

                    // Number row
                case KeyCode::Num0: return GLFW_KEY_0;
                case KeyCode::Num1: return GLFW_KEY_1;
                case KeyCode::Num2: return GLFW_KEY_2;
                case KeyCode::Num3: return GLFW_KEY_3;
                case KeyCode::Num4: return GLFW_KEY_4;
                case KeyCode::Num5: return GLFW_KEY_5;
                case KeyCode::Num6: return GLFW_KEY_6;
                case KeyCode::Num7: return GLFW_KEY_7;
                case KeyCode::Num8: return GLFW_KEY_8;
                case KeyCode::Num9: return GLFW_KEY_9;

                    // Function keys
                case KeyCode::F1:  return GLFW_KEY_F1;
                case KeyCode::F2:  return GLFW_KEY_F2;
                case KeyCode::F3:  return GLFW_KEY_F3;
                case KeyCode::F4:  return GLFW_KEY_F4;
                case KeyCode::F5:  return GLFW_KEY_F5;
                case KeyCode::F6:  return GLFW_KEY_F6;
                case KeyCode::F7:  return GLFW_KEY_F7;
                case KeyCode::F8:  return GLFW_KEY_F8;
                case KeyCode::F9:  return GLFW_KEY_F9;
                case KeyCode::F10: return GLFW_KEY_F10;
                case KeyCode::F11: return GLFW_KEY_F11;
                case KeyCode::F12: return GLFW_KEY_F12;

                    // Extended function keys
                case KeyCode::F13: return GLFW_KEY_F13;
                case KeyCode::F14: return GLFW_KEY_F14;
                case KeyCode::F15: return GLFW_KEY_F15;
                case KeyCode::F16: return GLFW_KEY_F16;
                case KeyCode::F17: return GLFW_KEY_F17;
                case KeyCode::F18: return GLFW_KEY_F18;
                case KeyCode::F19: return GLFW_KEY_F19;
                case KeyCode::F20: return GLFW_KEY_F20;
                case KeyCode::F21: return GLFW_KEY_F21;
                case KeyCode::F22: return GLFW_KEY_F22;
                case KeyCode::F23: return GLFW_KEY_F23;
                case KeyCode::F24: return GLFW_KEY_F24;

                    // Modifiers
                case KeyCode::LeftShift:    return GLFW_KEY_LEFT_SHIFT;
                case KeyCode::RightShift:   return GLFW_KEY_RIGHT_SHIFT;
                case KeyCode::LeftControl:  return GLFW_KEY_LEFT_CONTROL;
                case KeyCode::RightControl: return GLFW_KEY_RIGHT_CONTROL;
                case KeyCode::LeftAlt:      return GLFW_KEY_LEFT_ALT;
                case KeyCode::RightAlt:     return GLFW_KEY_RIGHT_ALT;
                case KeyCode::LeftSuper:    return GLFW_KEY_LEFT_SUPER;
                case KeyCode::RightSuper:   return GLFW_KEY_RIGHT_SUPER;

                    // Arrows
                case KeyCode::ArrowUp:    return GLFW_KEY_UP;
                case KeyCode::ArrowDown:  return GLFW_KEY_DOWN;
                case KeyCode::ArrowLeft:  return GLFW_KEY_LEFT;
                case KeyCode::ArrowRight: return GLFW_KEY_RIGHT;

                    // Navigation and editing
                case KeyCode::Home:     return GLFW_KEY_HOME;
                case KeyCode::End:      return GLFW_KEY_END;
                case KeyCode::PageUp:   return GLFW_KEY_PAGE_UP;
                case KeyCode::PageDown: return GLFW_KEY_PAGE_DOWN;
                case KeyCode::Insert:   return GLFW_KEY_INSERT;
                case KeyCode::Delete:   return GLFW_KEY_DELETE;

                    // Control
                case KeyCode::Escape:    return GLFW_KEY_ESCAPE;
                case KeyCode::Tab:       return GLFW_KEY_TAB;
                case KeyCode::Enter:     return GLFW_KEY_ENTER;
                case KeyCode::Backspace: return GLFW_KEY_BACKSPACE;
                case KeyCode::Space:     return GLFW_KEY_SPACE;

                    // Locks
                case KeyCode::CapsLock:   return GLFW_KEY_CAPS_LOCK;
                case KeyCode::ScrollLock: return GLFW_KEY_SCROLL_LOCK;
                case KeyCode::NumLock:    return GLFW_KEY_NUM_LOCK;

                    // Punctuation (US)
                case KeyCode::Grave:        return GLFW_KEY_GRAVE_ACCENT;
                case KeyCode::Minus:        return GLFW_KEY_MINUS;
                case KeyCode::Equal:        return GLFW_KEY_EQUAL;
                case KeyCode::LeftBracket:  return GLFW_KEY_LEFT_BRACKET;
                case KeyCode::RightBracket: return GLFW_KEY_RIGHT_BRACKET;
                case KeyCode::Backslash:    return GLFW_KEY_BACKSLASH;
                case KeyCode::Semicolon:    return GLFW_KEY_SEMICOLON;
                case KeyCode::Apostrophe:   return GLFW_KEY_APOSTROPHE;
                case KeyCode::Comma:        return GLFW_KEY_COMMA;
                case KeyCode::Period:       return GLFW_KEY_PERIOD;
                case KeyCode::Slash:        return GLFW_KEY_SLASH;

                    // Special/system
                case KeyCode::PrintScreen: return GLFW_KEY_PRINT_SCREEN;
                case KeyCode::Pause:       return GLFW_KEY_PAUSE;
                case KeyCode::Application: return GLFW_KEY_MENU;

                    // Numpad
                case KeyCode::Kp0:        return GLFW_KEY_KP_0;
                case KeyCode::Kp1:        return GLFW_KEY_KP_1;
                case KeyCode::Kp2:        return GLFW_KEY_KP_2;
                case KeyCode::Kp3:        return GLFW_KEY_KP_3;
                case KeyCode::Kp4:        return GLFW_KEY_KP_4;
                case KeyCode::Kp5:        return GLFW_KEY_KP_5;
                case KeyCode::Kp6:        return GLFW_KEY_KP_6;
                case KeyCode::Kp7:        return GLFW_KEY_KP_7;
                case KeyCode::Kp8:        return GLFW_KEY_KP_8;
                case KeyCode::Kp9:        return GLFW_KEY_KP_9;
                case KeyCode::KpDecimal:  return GLFW_KEY_KP_DECIMAL;
                case KeyCode::KpDivide:   return GLFW_KEY_KP_DIVIDE;
                case KeyCode::KpMultiply: return GLFW_KEY_KP_MULTIPLY;
                case KeyCode::KpSubtract: return GLFW_KEY_KP_SUBTRACT;
                case KeyCode::KpAdd:      return GLFW_KEY_KP_ADD;
                case KeyCode::KpEnter:    return GLFW_KEY_KP_ENTER;
                case KeyCode::KpEqual:    return GLFW_KEY_KP_EQUAL;

                case KeyCode::Unknown:
                case KeyCode::Count:
                default:
                    return -1; // invalid platform key
                }
            }

        }
    }
} // namespace Interstellar::Input::KeyCodeTranslator
