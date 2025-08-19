#include <gtest/gtest.h>
#include <GLFW/glfw3.h>

#include "Interstellar/Input/Keyboard/KeyboardManager.hpp"
#include "Interstellar/Input/KeyCodeTranslator/GlfwKeyMap.hpp"
#include "Interstellar/Input/KeyCode.hpp"

namespace interstellar::input {
    using namespace Interstellar::Input;
    using namespace Interstellar::Input::Keyboard;
    using namespace Interstellar::Input::KeyCodeTranslator;

    TEST(KeyboardManager, PressReleaseEdges)
    {
        GLFWwindow* win = nullptr; // not used
        GLFWKeyMap translator;
        KeyboardManager km(win, translator);

        // Frame 0: press A
        km.OnGlfwKey(GLFW_KEY_A, 0, GLFW_PRESS, 0);
        km.Update();

        EXPECT_TRUE(km.WasKeyPressed(KeyCode::A));
        EXPECT_FALSE(km.WasKeyReleased(KeyCode::A));
        EXPECT_TRUE(km.IsKeyDown(KeyCode::A));

        // Frame 1: no new events
        km.Update();
        EXPECT_FALSE(km.WasKeyPressed(KeyCode::A));
        EXPECT_FALSE(km.WasKeyReleased(KeyCode::A));
        EXPECT_TRUE(km.IsKeyDown(KeyCode::A));

        // Frame 2: release A
        km.OnGlfwKey(GLFW_KEY_A, 0, GLFW_RELEASE, 0);
        km.Update();
        EXPECT_FALSE(km.WasKeyPressed(KeyCode::A));
        EXPECT_TRUE(km.WasKeyReleased(KeyCode::A));
        EXPECT_FALSE(km.IsKeyDown(KeyCode::A));
    }

    TEST(KeyboardManager, RepeatIsDownButNotEdge)
    {
        GLFWwindow* win = nullptr;
        GLFWKeyMap translator;
        KeyboardManager km(win, translator);

        // Press
        km.OnGlfwKey(GLFW_KEY_SPACE, 0, GLFW_PRESS, 0);
        km.Update();
        EXPECT_TRUE(km.WasKeyPressed(KeyCode::Space));
        EXPECT_TRUE(km.IsKeyDown(KeyCode::Space));

        // Repeat should keep it down but not re-trigger the pressed edge on next frame
        km.OnGlfwKey(GLFW_KEY_SPACE, 0, GLFW_REPEAT, 0);
        km.Update();
        EXPECT_FALSE(km.WasKeyPressed(KeyCode::Space));
        EXPECT_TRUE(km.IsKeyDown(KeyCode::Space));
    }

    TEST(KeyboardManager, UnknownKeyIgnored)
    {
        GLFWwindow* win = nullptr;
        GLFWKeyMap translator;
        KeyboardManager km(win, translator);

        km.OnGlfwKey(0x7fffffff, 0, GLFW_PRESS, 0); // unmapped
        km.Update();

        // Spot check: A remains false
        EXPECT_FALSE(km.IsKeyDown(KeyCode::A));
        EXPECT_FALSE(km.WasKeyPressed(KeyCode::A));
        EXPECT_FALSE(km.WasKeyReleased(KeyCode::A));
    }
}