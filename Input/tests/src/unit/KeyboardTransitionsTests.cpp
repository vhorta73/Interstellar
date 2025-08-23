#include <gtest/gtest.h>
#include <Interstellar/Input/KeyCodes.hpp>
#include <Interstellar/Input/Core/KeyboardState.hpp>

using namespace Interstellar::Input;
using namespace Interstellar::Input::Core;

TEST(KeyboardTransitions, PressReleaseOneFrame) {
    KeyboardState kb;

    // Frame 1: nothing yet
    kb.beginFrame(0.016);
    EXPECT_FALSE(kb.isDown(KeyCode::Space));
    EXPECT_FALSE(kb.wasPressed(KeyCode::Space));
    EXPECT_FALSE(kb.wasReleased(KeyCode::Space));

    // Queue press during frame (simulated by backend)
    kb.setKeyDown(KeyCode::Space, true);

    // Frame 2: apply press
    kb.beginFrame(0.016);
    EXPECT_TRUE(kb.isDown(KeyCode::Space));
    EXPECT_TRUE(kb.wasPressed(KeyCode::Space));
    EXPECT_FALSE(kb.wasReleased(KeyCode::Space));

    // Still held in same frame: pressed flag should not persist next frame
    kb.beginFrame(0.016);
    EXPECT_TRUE(kb.isDown(KeyCode::Space));
    EXPECT_FALSE(kb.wasPressed(KeyCode::Space));
    EXPECT_FALSE(kb.wasReleased(KeyCode::Space));

    // Queue release
    kb.setKeyDown(KeyCode::Space, false);
    kb.beginFrame(0.016);
    EXPECT_FALSE(kb.isDown(KeyCode::Space));
    EXPECT_FALSE(kb.wasPressed(KeyCode::Space));
    EXPECT_TRUE(kb.wasReleased(KeyCode::Space));
}
