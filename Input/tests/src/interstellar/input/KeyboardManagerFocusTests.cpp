#include <gtest/gtest.h>
#include <GLFW/glfw3.h>
#include "Interstellar/Input/Keyboard/KeyboardManager.hpp"
#include "Interstellar/Input/KeyCodeTranslator/GlfwKeyMap.hpp"
#include "Interstellar/Input/KeyCode.hpp"

namespace interstellar::input {
    using namespace Interstellar::Input;
    using namespace Interstellar::Input::Keyboard;
    using namespace Interstellar::Input::KeyCodeTranslator;

    TEST(KeyboardManager_Focus, FocusLossClearsKeysNoEdges)
    {
        GLFWKeyMap tr;
        KeyboardManager km(nullptr, tr);

        // Press SPACE and publish
        km.OnGlfwKey(GLFW_KEY_SPACE, 0, GLFW_PRESS, 0);
        km.Update();
        EXPECT_TRUE(km.IsKeyDown(KeyCode::Space));

        // Focus lost clears state; should not emit a release edge spuriously
        km.OnFocusLost();
        km.Update();
        EXPECT_FALSE(km.IsKeyDown(KeyCode::Space));
        EXPECT_FALSE(km.WasKeyPressed(KeyCode::Space));
        EXPECT_FALSE(km.WasKeyReleased(KeyCode::Space));
    }

    TEST(KeyboardManager_Focus, AfterFocusLossNewPressEdgesCorrectly)
    {
        GLFWKeyMap tr;
        KeyboardManager km(nullptr, tr);

        km.OnFocusLost();
        km.Update();

        km.OnGlfwKey(GLFW_KEY_A, 0, GLFW_PRESS, 0);
        km.Update();
        EXPECT_TRUE(km.WasKeyPressed(KeyCode::A));
        EXPECT_TRUE(km.IsKeyDown(KeyCode::A));

        km.OnGlfwKey(GLFW_KEY_A, 0, GLFW_RELEASE, 0);
        km.Update();
        EXPECT_TRUE(km.WasKeyReleased(KeyCode::A));
        EXPECT_FALSE(km.IsKeyDown(KeyCode::A));
    }
}