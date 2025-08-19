#include <gtest/gtest.h>
#include <GLFW/glfw3.h>
#include "Interstellar/Input/KeyCodeTranslator/GlfwKeyMap.hpp"
#include "Interstellar/Input/KeyCode.hpp"


namespace interstelllar::input {
    using namespace Interstellar::Input;
    using namespace Interstellar::Input::KeyCodeTranslator;

    static void ExpectRoundTrip(GLFWKeyMap& t, int glfwKey, KeyCode expected)
    {
        // From platform
        KeyCode k = t.FromPlatform(glfwKey);
        EXPECT_EQ(k, expected) << "FromPlatform mismatch for GLFW key";

        // To platform (only if we expect a valid platform key)
        if (expected != KeyCode::Unknown)
        {
            int back = t.ToPlatform(expected);
            EXPECT_GE(back, 0) << "ToPlatform returned invalid for expected key";
            EXPECT_EQ(back, glfwKey) << "Round-trip mismatch";
        }
    }

    TEST(GlfwKeyMap, LettersDigitsBasic)
    {
        GLFWKeyMap t;
        ExpectRoundTrip(t, GLFW_KEY_A, KeyCode::A);
        ExpectRoundTrip(t, GLFW_KEY_Z, KeyCode::Z);

        ExpectRoundTrip(t, GLFW_KEY_0, KeyCode::Num0);
        ExpectRoundTrip(t, GLFW_KEY_9, KeyCode::Num9);
    }

    TEST(GlfwKeyMap, FunctionKeys)
    {
        GLFWKeyMap t;
        ExpectRoundTrip(t, GLFW_KEY_F1, KeyCode::F1);
        ExpectRoundTrip(t, GLFW_KEY_F12, KeyCode::F12);

#ifdef GLFW_KEY_F24
        ExpectRoundTrip(t, GLFW_KEY_F24, KeyCode::F24);
#endif
    }

    TEST(GlfwKeyMap, ModifiersAndArrows)
    {
        GLFWKeyMap t;
        ExpectRoundTrip(t, GLFW_KEY_LEFT_SHIFT, KeyCode::LeftShift);
        ExpectRoundTrip(t, GLFW_KEY_RIGHT_CONTROL, KeyCode::RightControl);
        ExpectRoundTrip(t, GLFW_KEY_LEFT_ALT, KeyCode::LeftAlt);

        // Super keys
#ifdef GLFW_KEY_LEFT_SUPER
        ExpectRoundTrip(t, GLFW_KEY_LEFT_SUPER, KeyCode::LeftSuper);
        ExpectRoundTrip(t, GLFW_KEY_RIGHT_SUPER, KeyCode::RightSuper);
#endif

        ExpectRoundTrip(t, GLFW_KEY_UP, KeyCode::ArrowUp);
        ExpectRoundTrip(t, GLFW_KEY_RIGHT, KeyCode::ArrowRight);
    }

    TEST(GlfwKeyMap, PunctuationAndSystem)
    {
        GLFWKeyMap t;
        ExpectRoundTrip(t, GLFW_KEY_GRAVE_ACCENT, KeyCode::Grave);
        ExpectRoundTrip(t, GLFW_KEY_MINUS, KeyCode::Minus);
        ExpectRoundTrip(t, GLFW_KEY_EQUAL, KeyCode::Equal);
        ExpectRoundTrip(t, GLFW_KEY_LEFT_BRACKET, KeyCode::LeftBracket);
        ExpectRoundTrip(t, GLFW_KEY_RIGHT_BRACKET, KeyCode::RightBracket);
        ExpectRoundTrip(t, GLFW_KEY_BACKSLASH, KeyCode::Backslash);
        ExpectRoundTrip(t, GLFW_KEY_SEMICOLON, KeyCode::Semicolon);
        ExpectRoundTrip(t, GLFW_KEY_APOSTROPHE, KeyCode::Apostrophe);
        ExpectRoundTrip(t, GLFW_KEY_COMMA, KeyCode::Comma);
        ExpectRoundTrip(t, GLFW_KEY_PERIOD, KeyCode::Period);
        ExpectRoundTrip(t, GLFW_KEY_SLASH, KeyCode::Slash);

#ifdef GLFW_KEY_PRINT_SCREEN
        ExpectRoundTrip(t, GLFW_KEY_PRINT_SCREEN, KeyCode::PrintScreen);
#endif
#ifdef GLFW_KEY_PAUSE
        ExpectRoundTrip(t, GLFW_KEY_PAUSE, KeyCode::Pause);
#endif
#ifdef GLFW_KEY_MENU
        ExpectRoundTrip(t, GLFW_KEY_MENU, KeyCode::Application);
#endif
    }

    TEST(GlfwKeyMap, Keypad)
    {
        GLFWKeyMap t;
        ExpectRoundTrip(t, GLFW_KEY_KP_0, KeyCode::Kp0);
        ExpectRoundTrip(t, GLFW_KEY_KP_9, KeyCode::Kp9);
        ExpectRoundTrip(t, GLFW_KEY_KP_DECIMAL, KeyCode::KpDecimal);
        ExpectRoundTrip(t, GLFW_KEY_KP_DIVIDE, KeyCode::KpDivide);
        ExpectRoundTrip(t, GLFW_KEY_KP_MULTIPLY, KeyCode::KpMultiply);
        ExpectRoundTrip(t, GLFW_KEY_KP_SUBTRACT, KeyCode::KpSubtract);
        ExpectRoundTrip(t, GLFW_KEY_KP_ADD, KeyCode::KpAdd);
        ExpectRoundTrip(t, GLFW_KEY_KP_ENTER, KeyCode::KpEnter);
#ifdef GLFW_KEY_KP_EQUAL
        ExpectRoundTrip(t, GLFW_KEY_KP_EQUAL, KeyCode::KpEqual);
#endif
    }

    TEST(GlfwKeyMap, UnknownsReturnUnknown)
    {
        GLFWKeyMap t;
        KeyCode k = t.FromPlatform(0x7fffffff); // arbitrary unmapped
        EXPECT_EQ(k, KeyCode::Unknown);

        int back = t.ToPlatform(KeyCode::Unknown);
        EXPECT_LT(back, 0);
    }
}