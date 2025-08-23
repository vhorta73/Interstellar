#include <gtest/gtest.h>
#include <Interstellar/Input/InputSystem.hpp>
#include <Interstellar/Input/IKeyboard.hpp>

using namespace Interstellar::Input;

TEST(Factory, CreatesNonNullWithNullBackend) {
    InputConfig cfg{};
    cfg.backend = InputConfig::Backend::Null;
    auto sys = InputSystem::Create(cfg);
    ASSERT_TRUE(sys != nullptr);
    EXPECT_EQ(sys->backend(), InputConfig::Backend::Null);
    EXPECT_FALSE(sys->keyboard().isDown(KeyCode::Space));
}

// Auto without a window: now guaranteed to fall back to Null even if GLFW is compiled.
TEST(Factory, AutoNoWindowFallsBackToNull) {
    InputConfig cfg{};
    cfg.backend = InputConfig::Backend::Auto;
    cfg.nativeWindow = nullptr;
    auto sys = InputSystem::Create(cfg);
    ASSERT_TRUE(sys != nullptr);
    EXPECT_EQ(sys->backend(), InputConfig::Backend::Null);
}

#ifdef INTERSTELLAR_INPUT_HAVE_GLFW
#include <GLFW/glfw3.h>

// Auto with a window: prefers GLFW when compiled in.
TEST(Factory, AutoWithGLFWWindowSelectsGLFW) {
    ASSERT_TRUE(glfwInit());
    GLFWwindow* w = glfwCreateWindow(320, 200, "input-test", nullptr, nullptr);
    ASSERT_TRUE(w != nullptr);

    InputConfig cfg{};
    cfg.backend = InputConfig::Backend::Auto;
    cfg.nativeWindow = w;

    auto sys = InputSystem::Create(cfg);
    ASSERT_TRUE(sys != nullptr);
    EXPECT_EQ(sys->backend(), InputConfig::Backend::GLFW);

    sys->pump();
    sys->beginFrame(0.016);
    sys->endFrame();

    glfwDestroyWindow(w);
    glfwTerminate();
}

// Explicit GLFW request still works when a window is provided.
TEST(Factory, CreatesGLFWAndDrivesFrame) {
    ASSERT_TRUE(glfwInit());
    GLFWwindow* w = glfwCreateWindow(320, 200, "input-test", nullptr, nullptr);
    ASSERT_TRUE(w != nullptr);

    InputConfig cfg{};
    cfg.backend = InputConfig::Backend::GLFW;
    cfg.nativeWindow = w;

    auto sys = InputSystem::Create(cfg);
    ASSERT_TRUE(sys != nullptr);
    EXPECT_EQ(sys->backend(), InputConfig::Backend::GLFW);

    sys->pump();
    sys->beginFrame(0.016);
    sys->endFrame();

    glfwDestroyWindow(w);
    glfwTerminate();
}
#endif
