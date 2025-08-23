#include <gtest/gtest.h>
#include <Interstellar/Input/KeyCodes.hpp>
#include <Interstellar/Input/Core/KeyboardState.hpp>

using namespace Interstellar::Input;
using namespace Interstellar::Input::Core;

TEST(KeyboardTransitions, PressReleaseOneFrame) {
    KeyboardState kb;
    kb.beginFrame(0.016);
    EXPECT_FALSE(kb.isDown(KeyCode::Space));
    EXPECT_FALSE(kb.wasPressed(KeyCode::Space));
    EXPECT_FALSE(kb.wasReleased(KeyCode::Space));

    kb.setKeyDown(KeyCode::Space, true);
    kb.beginFrame(0.016);
    EXPECT_TRUE(kb.isDown(KeyCode::Space));
    EXPECT_TRUE(kb.wasPressed(KeyCode::Space));
    EXPECT_FALSE(kb.wasReleased(KeyCode::Space));

    kb.beginFrame(0.016);
    EXPECT_TRUE(kb.isDown(KeyCode::Space));
    EXPECT_FALSE(kb.wasPressed(KeyCode::Space));
    EXPECT_FALSE(kb.wasReleased(KeyCode::Space));

    kb.setKeyDown(KeyCode::Space, false);
    kb.beginFrame(0.016);
    EXPECT_FALSE(kb.isDown(KeyCode::Space));
    EXPECT_FALSE(kb.wasPressed(KeyCode::Space));
    EXPECT_TRUE(kb.wasReleased(KeyCode::Space));
}

TEST(KeyboardTransitions, MultipleKeysIndependent) {
    KeyboardState kb;
    kb.beginFrame(0.016);

    kb.setKeyDown(KeyCode::A, true);
    kb.setKeyDown(KeyCode::D, false);
    kb.beginFrame(0.016);
    EXPECT_TRUE(kb.wasPressed(KeyCode::A));
    EXPECT_FALSE(kb.wasPressed(KeyCode::D));
    EXPECT_TRUE(kb.isDown(KeyCode::A));
    EXPECT_FALSE(kb.isDown(KeyCode::D));

    // Repeat press should not retrigger wasPressed next frame if still held
    kb.beginFrame(0.016);
    EXPECT_FALSE(kb.wasPressed(KeyCode::A));
    EXPECT_TRUE(kb.isDown(KeyCode::A));
}
