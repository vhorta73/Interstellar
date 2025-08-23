#include <gtest/gtest.h>
#include <Interstellar/Input/Convenience.hpp>
#include <Interstellar/Input/Core/KeyboardState.hpp>

using namespace Interstellar::Input;
using namespace Interstellar::Input::Core;

TEST(Bindings, PressDownReleaseAcrossBindings) {
    KeyboardState kb;
    InputContext ctx;
    ctx.bind("Jump", KeyCode::Space);
    ctx.bind("Jump", KeyCode::W); // duplicate action with another key

    kb.beginFrame(0.016);
    EXPECT_FALSE(ctx.actionPressed(kb, "Jump"));

    kb.setKeyDown(KeyCode::W, true);
    kb.beginFrame(0.016);
    EXPECT_TRUE(ctx.actionPressed(kb, "Jump"));
    EXPECT_TRUE(ctx.actionDown(kb, "Jump"));

    kb.beginFrame(0.016);
    EXPECT_FALSE(ctx.actionPressed(kb, "Jump")); // pressed does not persist

    kb.setKeyDown(KeyCode::W, false);
    kb.beginFrame(0.016);
    EXPECT_TRUE(ctx.actionReleased(kb, "Jump"));
    EXPECT_FALSE(ctx.actionDown(kb, "Jump"));
}
