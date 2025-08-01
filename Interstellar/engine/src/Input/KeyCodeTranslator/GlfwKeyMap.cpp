#include "Input/KeyCodeTranslator/GlfwKeyMap.hpp"
#include "Interstellar/Input/KeyCodeTranslator/IKeyCodeTranslator.hpp"

#include <GLFW/glfw3.h>
#include <unordered_map>

using namespace Interstellar::Input::KeyCodeTranslator;
using namespace Interstellar::Input;


KeyCode GLFWKeyMap::FromPlatform(int glfwKey) const {
    static const std::unordered_map<int, KeyCode> map = {
        {GLFW_KEY_A, KeyCode::A},
        {GLFW_KEY_B, KeyCode::B},
        {GLFW_KEY_C, KeyCode::C},
        {GLFW_KEY_D, KeyCode::D},
        {GLFW_KEY_E, KeyCode::E},
        {GLFW_KEY_F, KeyCode::F},
        {GLFW_KEY_G, KeyCode::G},
        {GLFW_KEY_H, KeyCode::H},
        {GLFW_KEY_I, KeyCode::I},
        {GLFW_KEY_J, KeyCode::J},
        {GLFW_KEY_K, KeyCode::K},
        {GLFW_KEY_L, KeyCode::L},
        {GLFW_KEY_M, KeyCode::M},
        {GLFW_KEY_N, KeyCode::N},
        {GLFW_KEY_O, KeyCode::O},
        {GLFW_KEY_P, KeyCode::P},
        {GLFW_KEY_Q, KeyCode::Q},
        {GLFW_KEY_R, KeyCode::R},
        {GLFW_KEY_S, KeyCode::S},
        {GLFW_KEY_T, KeyCode::T},
        {GLFW_KEY_U, KeyCode::U},
        {GLFW_KEY_V, KeyCode::V},
        {GLFW_KEY_W, KeyCode::W},
        {GLFW_KEY_X, KeyCode::X},
        {GLFW_KEY_Y, KeyCode::Y},
        {GLFW_KEY_Z, KeyCode::Z},
        {GLFW_KEY_0, KeyCode::Num0},
        {GLFW_KEY_1, KeyCode::Num1},
        {GLFW_KEY_2, KeyCode::Num2},
        {GLFW_KEY_3, KeyCode::Num3},
        {GLFW_KEY_4, KeyCode::Num4},
        {GLFW_KEY_5, KeyCode::Num5},
        {GLFW_KEY_6, KeyCode::Num6},
        {GLFW_KEY_7, KeyCode::Num7},
        {GLFW_KEY_8, KeyCode::Num8},
        {GLFW_KEY_9, KeyCode::Num9},
        {GLFW_KEY_F1, KeyCode::F1},
        {GLFW_KEY_F2, KeyCode::F2},
        {GLFW_KEY_F3, KeyCode::F3},
        {GLFW_KEY_F4, KeyCode::F4},
        {GLFW_KEY_F5, KeyCode::F5},
        {GLFW_KEY_F6, KeyCode::F6},
        {GLFW_KEY_F7, KeyCode::F7},
        {GLFW_KEY_F8, KeyCode::F8},
        {GLFW_KEY_F9, KeyCode::F9},
        {GLFW_KEY_F10, KeyCode::F10},
        {GLFW_KEY_F11, KeyCode::F11},
        {GLFW_KEY_F12, KeyCode::F12},
        {GLFW_KEY_LEFT_SHIFT, KeyCode::LeftShift},
        {GLFW_KEY_RIGHT_SHIFT, KeyCode::RightShift},
        {GLFW_KEY_LEFT_CONTROL, KeyCode::LeftControl},
        {GLFW_KEY_RIGHT_CONTROL, KeyCode::RightControl},
        {GLFW_KEY_LEFT_ALT, KeyCode::LeftAlt},
        {GLFW_KEY_RIGHT_ALT, KeyCode::RightAlt},
        {GLFW_KEY_UP, KeyCode::ArrowUp},
        {GLFW_KEY_DOWN, KeyCode::ArrowDown},
        {GLFW_KEY_LEFT, KeyCode::ArrowLeft},
        {GLFW_KEY_RIGHT, KeyCode::ArrowRight},
        {GLFW_KEY_HOME, KeyCode::Home},
        {GLFW_KEY_END, KeyCode::End},
        {GLFW_KEY_PAGE_UP, KeyCode::PageUp},
        {GLFW_KEY_PAGE_DOWN, KeyCode::PageDown},
        {GLFW_KEY_INSERT, KeyCode::Insert},
        {GLFW_KEY_DELETE, KeyCode::Delete},
        {GLFW_KEY_ESCAPE, KeyCode::Escape},
        {GLFW_KEY_TAB, KeyCode::Tab},
        {GLFW_KEY_ENTER, KeyCode::Enter},
        {GLFW_KEY_BACKSPACE, KeyCode::Backspace},
        {GLFW_KEY_SPACE, KeyCode::Space},
        {GLFW_KEY_CAPS_LOCK, KeyCode::CapsLock},
        {GLFW_KEY_SCROLL_LOCK, KeyCode::ScrollLock},
        {GLFW_KEY_NUM_LOCK, KeyCode::NumLock},
        {GLFW_KEY_MINUS, KeyCode::Minus},
        {GLFW_KEY_EQUAL, KeyCode::Equal},
        {GLFW_KEY_LEFT_BRACKET, KeyCode::LeftBracket},
        {GLFW_KEY_RIGHT_BRACKET, KeyCode::RightBracket},
        {GLFW_KEY_BACKSLASH, KeyCode::Backslash},
        {GLFW_KEY_SEMICOLON, KeyCode::Semicolon},
        {GLFW_KEY_APOSTROPHE, KeyCode::Apostrophe},
        {GLFW_KEY_COMMA, KeyCode::Comma},
        {GLFW_KEY_PERIOD, KeyCode::Period},
        {GLFW_KEY_SLASH, KeyCode::Slash},
        {GLFW_KEY_PRINT_SCREEN, KeyCode::PrintScreen},
        {GLFW_KEY_PAUSE, KeyCode::Pause},
        {GLFW_KEY_MENU, KeyCode::Application},
    };

    auto it = map.find(glfwKey);
    return it != map.end() ? it->second : KeyCode::Unknown;
}


int GLFWKeyMap::ToPlatform(KeyCode code) const {
    static const std::unordered_map<KeyCode, int> reverseMap = [] {
        std::unordered_map<KeyCode, int> map;

        GLFWKeyMap translator;
        for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_MENU; ++key) {
            KeyCode k = translator.FromPlatform(key);
            if (k != KeyCode::Unknown)
                map[k] = key;
        }
        return map;
        }();

    auto it = reverseMap.find(code);
    return it != reverseMap.end() ? it->second : -1;
}
