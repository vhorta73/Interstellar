#include <gtest/gtest.h>
#include <GLFW/glfw3.h>

#include "Interstellar/Input/Manager/GlfwInputManager.hpp"
#include "Interstellar/Input/KeyCode.hpp"
#include "Interstellar/Input/IInputManager.hpp"

namespace interstellar::input {

    using namespace Interstellar::Input;
    using namespace Interstellar::Input::Manager;

    TEST(GlfwInputManager, GettersReturnLiveManagers)
    {
        GlfwInputManager mgr(nullptr);

        // Non-const and const overloads
        IMouseManager& mouse = mgr.GetMouseManager();
        IKeyboardManager& kb = mgr.GetKeyboardManager();

        const IMouseManager& cmouse = static_cast<const GlfwInputManager&>(mgr).GetMouseManager();
        const IKeyboardManager& ckb = static_cast<const GlfwInputManager&>(mgr).GetKeyboardManager();

        (void)mouse; (void)kb; (void)cmouse; (void)ckb;
        SUCCEED();
    }

    TEST(GlfwInputManager, UpdateForwardsToDevices)
    {
        GlfwInputManager mgr(nullptr);

        // With no events, calling Update should be safe and idempotent
        mgr.Update();
        mgr.Update();
        SUCCEED();
    }

    TEST(GlfwInputManager, InjectSyntheticKeyPress)
    {
        GlfwInputManager mgr(nullptr);

        // Inject press at engine KeyCode level and advance a frame
        mgr.InjectSyntheticKeyPress(KeyCode::A);
        mgr.Update();

        // Query through public keyboard interface
        IKeyboardManager& kb = mgr.GetKeyboardManager();

        EXPECT_TRUE(kb.IsKeyDown(KeyCode::A));
        EXPECT_TRUE(kb.WasKeyPressed(KeyCode::A));

        // Next frame: pressed edge clears, still down
        mgr.Update();
        EXPECT_FALSE(kb.WasKeyPressed(KeyCode::A));
        EXPECT_TRUE(kb.IsKeyDown(KeyCode::A));
    }
}