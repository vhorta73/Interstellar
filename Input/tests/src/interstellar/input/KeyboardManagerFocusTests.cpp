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

    TEST(GlfwKeyMap, ArrowKeysAreMapped)
    {
        using namespace Interstellar::Input;
        using namespace Interstellar::Input::KeyCodeTranslator;

        GLFWKeyMap tr;
        EXPECT_EQ(tr.FromPlatform(GLFW_KEY_LEFT), KeyCode::ArrowLeft);
        EXPECT_EQ(tr.FromPlatform(GLFW_KEY_RIGHT), KeyCode::ArrowRight);
        EXPECT_EQ(tr.FromPlatform(GLFW_KEY_UP), KeyCode::ArrowUp);
        EXPECT_EQ(tr.FromPlatform(GLFW_KEY_DOWN), KeyCode::ArrowDown);
    }

    TEST(KeyboardManager_Edges, PressReleaseAndRepeat)
    {
        using namespace Interstellar::Input;
        using namespace Interstellar::Input::Keyboard;
        using namespace Interstellar::Input::KeyCodeTranslator;

        GLFWKeyMap tr;
        KeyboardManager km(nullptr, tr);

        // PRESS (not published yet)
        km.OnGlfwKey(GLFW_KEY_LEFT, 0, GLFW_PRESS, 0);
        EXPECT_FALSE(km.IsKeyDown(KeyCode::ArrowLeft));   // not yet published

        // Publish -> down & pressed edge
        km.Update();
        EXPECT_TRUE(km.IsKeyDown(KeyCode::ArrowLeft));
        EXPECT_TRUE(km.WasKeyPressed(KeyCode::ArrowLeft));
        EXPECT_FALSE(km.WasKeyReleased(KeyCode::ArrowLeft));

        // REPEAT: stays down, no new "pressed" edge after publish
        km.OnGlfwKey(GLFW_KEY_LEFT, 0, GLFW_REPEAT, 0);
        km.Update();
        EXPECT_TRUE(km.IsKeyDown(KeyCode::ArrowLeft));
        EXPECT_FALSE(km.WasKeyPressed(KeyCode::ArrowLeft));
        EXPECT_FALSE(km.WasKeyReleased(KeyCode::ArrowLeft));

        // RELEASE -> up & released edge
        km.OnGlfwKey(GLFW_KEY_LEFT, 0, GLFW_RELEASE, 0);
        km.Update();
        EXPECT_FALSE(km.IsKeyDown(KeyCode::ArrowLeft));
        EXPECT_FALSE(km.WasKeyPressed(KeyCode::ArrowLeft));
        EXPECT_TRUE(km.WasKeyReleased(KeyCode::ArrowLeft));
    }

    TEST(InputIntegration, CreateSetsCallbacksAndUserPointer)
    {
        using namespace Interstellar::Input;

        ASSERT_TRUE(glfwInit());

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        GLFWwindow* win = glfwCreateWindow(320, 200, "test", nullptr, nullptr);
        ASSERT_NE(win, nullptr);

        // Create your input manager (this should install callbacks)
        auto input = IInputManager::Create(win);
        ASSERT_TRUE(input);

        // Assert there is *some* user pointer set (hub or manager)
        void* up = glfwGetWindowUserPointer(win);
        EXPECT_NE(up, nullptr);

        // Replacing a GLFW callback returns the previous pointer; it must be non-null
        auto dummyKey = glfwSetKeyCallback(win, nullptr);
        auto dummyMouse = glfwSetMouseButtonCallback(win, nullptr);
        auto dummyScroll = glfwSetScrollCallback(win, nullptr);
        auto dummyCursor = glfwSetCursorPosCallback(win, nullptr);

        EXPECT_NE(dummyKey, nullptr) << "Key callback was not installed";
        EXPECT_NE(dummyMouse, nullptr) << "Mouse button callback was not installed";
        EXPECT_NE(dummyScroll, nullptr) << "Scroll callback was not installed";
        EXPECT_NE(dummyCursor, nullptr) << "Cursor-pos callback was not installed";

        // Put them back so the window can continue to be used if needed
        glfwSetKeyCallback(win, dummyKey);
        glfwSetMouseButtonCallback(win, dummyMouse);
        glfwSetScrollCallback(win, dummyScroll);
        glfwSetCursorPosCallback(win, dummyCursor);

        glfwDestroyWindow(win);
        glfwTerminate();
    }

    TEST(InputIntegration, InstalledKeyCallbackDrivesKeyboardManager)
    {
        using namespace Interstellar::Input;

        ASSERT_TRUE(glfwInit());
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        GLFWwindow* win = glfwCreateWindow(320, 200, "test", nullptr, nullptr);
        ASSERT_NE(win, nullptr);

        auto input = IInputManager::Create(win);
        ASSERT_TRUE(input);

        // Get the callback that Create(win) installed
        auto keycb = glfwSetKeyCallback(win, nullptr);
        ASSERT_NE(keycb, nullptr) << "Key callback not installed by IInputManager::Create";

        // Restore it so the window keeps working
        glfwSetKeyCallback(win, keycb);

        // Simulate a Left Arrow press -> publish -> check high level
        keycb(win, GLFW_KEY_LEFT, /*sc=*/0, GLFW_PRESS, /*mods=*/0);
        input->Update();

        auto& kb = input->GetKeyboardManager();
        EXPECT_TRUE(kb.IsKeyDown(KeyCode::ArrowLeft));
        EXPECT_TRUE(kb.WasKeyPressed(KeyCode::ArrowLeft));

        // Simulate release -> publish -> check
        keycb(win, GLFW_KEY_LEFT, /*sc=*/0, GLFW_RELEASE, /*mods=*/0);
        input->Update();
        EXPECT_FALSE(kb.IsKeyDown(KeyCode::ArrowLeft));
        EXPECT_TRUE(kb.WasKeyReleased(KeyCode::ArrowLeft));

        glfwDestroyWindow(win);
        glfwTerminate();
    }

    TEST(KeyboardManager_Safety, KeyBufferCoversEnumRange)
    {
        using namespace Interstellar::Input;
        using namespace Interstellar::Input::Keyboard;
        using namespace Interstellar::Input::KeyCodeTranslator;

        GLFWKeyMap tr;
        KeyboardManager km(nullptr, tr);

        // static_cast is allowed because Count is the sentinel value
        constexpr std::size_t enumCount = static_cast<std::size_t>(KeyCode::Count);

        // If you expose km.KeyCount() or a trait, compare it here:
        // EXPECT_EQ(km.KeyCount(), enumCount);
        // Otherwise, at least ensure common high enum values are accepted:
        km.OnGlfwKey(GLFW_KEY_F24, 0, GLFW_PRESS, 0);
        km.Update();
        // Doesn't assert a specific key, but will crash or drop if buffers are undersized.
        SUCCEED();
    }

}