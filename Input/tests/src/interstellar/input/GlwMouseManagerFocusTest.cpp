#include <gtest/gtest.h>
#include <GLFW/glfw3.h>
#include "Interstellar/Input/Mouse/GlfwMouseManager.hpp"
#include "Interstellar/Input/MouseButton.hpp"

namespace interstellar::input {
    using namespace Interstellar::Input;
    using namespace Interstellar::Input::Mouse;

    TEST(GlfwMouseManager_Focus, FocusLossClearsButtonsDragAndScroll)
    {
        GlfwMouseManager m(nullptr);

        // Establish some non-zero state
        m.OnGlfwMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
        m.OnGlfwCursorPos(0.0, 0.0);
        m.Update(); // publish press

        m.OnGlfwCursorPos(4.0, 0.0); // move enough to trigger drag next frame
        m.Update();
        EXPECT_TRUE(m.IsButtonDown(MouseButton::Left));
        EXPECT_TRUE(m.IsDragging());
        m.OnGlfwScroll(2.0, -1.0);
        EXPECT_DOUBLE_EQ(m.GetScrollOffsetX(), 2.0);
        EXPECT_DOUBLE_EQ(m.GetScrollOffsetY(), -1.0);

        // Simulate alt-tab / focus loss
        m.OnGlfwFocusLost();
        m.Update(); // publish cleared state

        // All cleared; edges should not fire spuriously
        EXPECT_FALSE(m.IsButtonDown(MouseButton::Left));
        EXPECT_FALSE(m.WasButtonPressed(MouseButton::Left));
        EXPECT_FALSE(m.WasButtonReleased(MouseButton::Left));
        EXPECT_FALSE(m.IsDragging());
        EXPECT_DOUBLE_EQ(m.GetScrollOffsetX(), 0.0);
        EXPECT_DOUBLE_EQ(m.GetScrollOffsetY(), 0.0);
    }

    TEST(GlfwMouseManager_Focus, AfterFocusLossNewPressEdgesCorrectly)
    {
        GlfwMouseManager m(nullptr);

        // Lose focus while neutral
        m.OnGlfwFocusLost();
        m.Update();

        // First user input after focus regain should behave normally
        m.OnGlfwMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
        m.Update();
        EXPECT_TRUE(m.WasButtonPressed(MouseButton::Left));
        EXPECT_TRUE(m.IsButtonDown(MouseButton::Left));

        // Release edge works too
        m.OnGlfwMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, 0);
        m.Update();
        EXPECT_TRUE(m.WasButtonReleased(MouseButton::Left));
        EXPECT_FALSE(m.IsButtonDown(MouseButton::Left));
    }
}