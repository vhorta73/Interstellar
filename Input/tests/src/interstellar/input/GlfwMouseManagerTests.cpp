#include <gtest/gtest.h>
#include <GLFW/glfw3.h>
#include "Interstellar/Input/Mouse/GlfwMouseManager.hpp"
#include "Interstellar/Input/MouseButton.hpp"

namespace interstellar::input {
    using namespace Interstellar::Input;
    using namespace Interstellar::Input::Mouse;

    TEST(GlfwMouseManager, PositionUpdates)
    {
        GlfwMouseManager m(nullptr);

        m.OnGlfwCursorPos(10.0, 20.0);
        EXPECT_DOUBLE_EQ(m.GetX(), 10.0);
        EXPECT_DOUBLE_EQ(m.GetY(), 20.0);

        m.Update(); // capture last pos

        m.OnGlfwCursorPos(12.0, 21.0);
        m.Update();

        EXPECT_DOUBLE_EQ(m.GetX(), 12.0);
        EXPECT_DOUBLE_EQ(m.GetY(), 21.0);
    }

    TEST(GlfwMouseManager, ButtonEdges)
    {
        GlfwMouseManager m(nullptr);

        // Press left
        m.OnGlfwMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
        m.Update();

        EXPECT_TRUE(m.WasButtonPressed(MouseButton::Left));
        EXPECT_TRUE(m.IsButtonDown(MouseButton::Left));
        EXPECT_FALSE(m.WasButtonReleased(MouseButton::Left));

        // Next frame, no event: pressed edge resets, state remains down
        m.Update();
        EXPECT_FALSE(m.WasButtonPressed(MouseButton::Left));
        EXPECT_TRUE(m.IsButtonDown(MouseButton::Left));

        // Release
        m.OnGlfwMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, 0);
        m.Update();
        EXPECT_TRUE(m.WasButtonReleased(MouseButton::Left));
        EXPECT_FALSE(m.IsButtonDown(MouseButton::Left));
    }

    TEST(GlfwMouseManager, DragDetection)
    {
        GlfwMouseManager m(nullptr);

        // Hold left and move > threshold
        m.OnGlfwMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
        m.OnGlfwCursorPos(0.0, 0.0);
        m.Update(); // set last pos

        m.OnGlfwCursorPos(3.0, 0.0); // about 3px move, threshold is 1.5
        m.Update();
        EXPECT_TRUE(m.IsDragging());

        // Release ends drag
        m.OnGlfwMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, 0);
        m.Update();
        EXPECT_FALSE(m.IsDragging());
    }

    TEST(GlfwMouseManager, ScrollAccumulationAndReset)
    {
        GlfwMouseManager m(nullptr);

        m.OnGlfwScroll(2.0, -1.0); // x right, y down
        EXPECT_DOUBLE_EQ(m.GetScrollOffsetX(), 2.0);
        EXPECT_DOUBLE_EQ(m.GetScrollOffsetY(), -1.0);

        // Accumulates
        m.OnGlfwScroll(-0.5, 0.5);
        EXPECT_DOUBLE_EQ(m.GetScrollOffsetX(), 1.5);
        EXPECT_DOUBLE_EQ(m.GetScrollOffsetY(), -0.5);

        // Reset
        m.ResetScrollOffsets();
        EXPECT_DOUBLE_EQ(m.GetScrollOffsetX(), 0.0);
        EXPECT_DOUBLE_EQ(m.GetScrollOffsetY(), 0.0);
    }
}