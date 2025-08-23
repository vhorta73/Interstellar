#include <gtest/gtest.h>
#include <Interstellar/Input/Core/MouseState.hpp>

using namespace Interstellar::Input;
using namespace Interstellar::Input::Core;

TEST(MouseDelta, PositionAndDelta) {
    MouseState m;
    m.beginFrame(0.016);
    EXPECT_FLOAT_EQ(m.position().x, 0);
    EXPECT_FLOAT_EQ(m.position().y, 0);
    EXPECT_FLOAT_EQ(m.delta().dx, 0);
    EXPECT_FLOAT_EQ(m.delta().dy, 0);

    m.setPosition(100.0f, 50.0f);
    m.beginFrame(0.016);
    EXPECT_FLOAT_EQ(m.position().x, 100.0f);
    EXPECT_FLOAT_EQ(m.position().y, 50.0f);
    EXPECT_FLOAT_EQ(m.delta().dx, 100.0f);
    EXPECT_FLOAT_EQ(m.delta().dy, 50.0f);

    m.beginFrame(0.016);
    EXPECT_FLOAT_EQ(m.delta().dx, 0.0f);
    EXPECT_FLOAT_EQ(m.delta().dy, 0.0f);
}

TEST(MouseWheelAndRelative, AccumulateAndReset) {
    MouseState m;
    m.beginFrame(0.016);
    EXPECT_FLOAT_EQ(m.wheel().x, 0.0f);
    EXPECT_FLOAT_EQ(m.wheel().y, 0.0f);

    m.addWheel(1.0f, -2.0f);
    m.addWheel(0.0f, 0.5f);
    m.beginFrame(0.016);
    EXPECT_FLOAT_EQ(m.wheel().x, 1.0f);
    EXPECT_FLOAT_EQ(m.wheel().y, -1.5f);

    // After another beginFrame without new wheel, it should reset to zero
    m.beginFrame(0.016);
    EXPECT_FLOAT_EQ(m.wheel().x, 0.0f);
    EXPECT_FLOAT_EQ(m.wheel().y, 0.0f);

    // Relative mode toggle
    EXPECT_FALSE(m.relativeMode());
    m.setRelativeMode(true);
    EXPECT_TRUE(m.relativeMode());
    m.setRelativeMode(false);
    EXPECT_FALSE(m.relativeMode());
}
