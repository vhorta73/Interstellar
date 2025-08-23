#include <gtest/gtest.h>
#include <Interstellar/Input/Core/MouseState.hpp>

using namespace Interstellar::Input;
using namespace Interstellar::Input::Core;

TEST(MouseDelta, PositionAndDelta) {
    MouseState m;

    // Initialize
    m.beginFrame(0.016);
    EXPECT_FLOAT_EQ(m.position().x, 0);
    EXPECT_FLOAT_EQ(m.position().y, 0);
    EXPECT_FLOAT_EQ(m.delta().dx, 0);
    EXPECT_FLOAT_EQ(m.delta().dy, 0);

    // Move during frame
    m.setPosition(100.0f, 50.0f);
    m.beginFrame(0.016);
    EXPECT_FLOAT_EQ(m.position().x, 100.0f);
    EXPECT_FLOAT_EQ(m.position().y, 50.0f);
    EXPECT_FLOAT_EQ(m.delta().dx, 100.0f);
    EXPECT_FLOAT_EQ(m.delta().dy, 50.0f);

    // Next frame, no move -> delta = 0
    m.beginFrame(0.016);
    EXPECT_FLOAT_EQ(m.delta().dx, 0.0f);
    EXPECT_FLOAT_EQ(m.delta().dy, 0.0f);
}
