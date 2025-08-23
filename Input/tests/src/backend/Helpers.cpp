#include <gtest/gtest.h>
#include <Interstellar/Input/Mouse.hpp>            // DeltaNorm, WheelZoom, MouseFilter
#include <Interstellar/Input/Core/MouseState.hpp>

using namespace Interstellar::Input;
using namespace Interstellar::Input::Core;

TEST(Helpers, DeltaNormBoundsAndZeroSize) {
    MouseState m;

    // No window size -> zero
    EXPECT_EQ(DeltaNorm(m, 0, 0), glm::vec2(0.0f, 0.0f));

    // Move 100x50 over 200x100 -> dx=1.0, dy=-1.0 (Y up)
    m.setPosition(100.0f, 50.0f);
    m.beginFrame(0.016); // commit position
    // Next frame no move; delta is 0
    m.beginFrame(0.016);
    // simulate a movement
    m.setPosition(200.0f, 100.0f);
    m.beginFrame(0.016);
    glm::vec2 nd = DeltaNorm(m, 200, 100);
    EXPECT_FLOAT_EQ(nd.x, 2.0f * (200.0f - 200.0f) / 200.0f); // 0
    EXPECT_FLOAT_EQ(nd.y, -2.0f * (100.0f - 100.0f) / 100.0f); // 0

    // Now move by half window
    m.setPosition(300.0f, 150.0f); // +100,+50
    m.beginFrame(0.016);
    nd = DeltaNorm(m, 200, 100);
    EXPECT_FLOAT_EQ(nd.x, 2.0f * (100.0f) / 200.0f); // +1.0
    EXPECT_FLOAT_EQ(nd.y, -2.0f * (50.0f) / 100.0f); // -1.0
}

TEST(Helpers, WheelZoomClampsAndSteps) {
    MouseState m;

    // No wheel -> unchanged
    EXPECT_FLOAT_EQ(WheelZoom(m, 10.0f), 10.0f);

    // One tick up on Y with default step 0.5
    m.addWheel(0.0f, 1.0f);
    m.beginFrame(0.016);
    EXPECT_FLOAT_EQ(WheelZoom(m, 10.0f), 10.5f);

    // Clamp to min/max
    m.addWheel(0.0f, -1000.0f);
    m.beginFrame(0.016);
    EXPECT_FLOAT_EQ(WheelZoom(m, 1.0f, 0.5f, 0.1f, 5.0f), 0.1f);
}

TEST(Helpers, MouseFilterSmoothing) {
    MouseState m;
    m.setPosition(0.0f, 0.0f);
    m.beginFrame(0.016);
    m.setPosition(100.0f, 0.0f); // move in +x
    m.beginFrame(0.016);

    MouseFilter filter;
    filter.smoothing = 0.5f;

    glm::vec2 v1 = filter.apply(m, 200, 100); // norm: x ~ +1.0
    EXPECT_NEAR(v1.x, 1.0f * (1.0f - (1.0f - 0.5f)), 1e-4); // 0.5
    EXPECT_NEAR(v1.y, 0.0f, 1e-4);

    // Next identical delta -> exponential smoothing accumulates
    m.setPosition(200.0f, 0.0f);
    m.beginFrame(0.016);
    glm::vec2 v2 = filter.apply(m, 200, 100);
    EXPECT_GT(v2.x, v1.x);
    filter.reset();
    EXPECT_EQ(filter.prev, glm::vec2(0.0f, 0.0f));
}
