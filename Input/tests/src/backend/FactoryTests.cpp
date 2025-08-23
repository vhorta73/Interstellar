#include <gtest/gtest.h>
#include <Interstellar/Input/InputSystem.hpp>
#include <Interstellar/Input/IKeyboard.hpp>

using namespace Interstellar::Input;

TEST(Factory, CreatesNonNullWithNullBackend) {
    InputConfig cfg;
    cfg.backend = InputConfig::Backend::Null;
    auto sys = InputSystem::Create(cfg);
    ASSERT_TRUE(sys != nullptr);
    // Default state: nothing pressed
    EXPECT_FALSE(sys->keyboard().isDown(KeyCode::Space));
}

#ifdef ISTELLAR_INPUT_HAVE_GLFW
#include <GLFW/glfw3.h>

TEST(Factory, CreatesGLFWAndPumps) {
    ASSERT_TRUE(glfwInit());
    GLFWwindow* w = glfwCreateWindow(320, 200, "input-test", nullptr, nullptr);
    ASSERT_TRUE(w != nullptr);

    InputConfig cfg;
    cfg.backend = InputConfig::Backend::GLFW;
    cfg.nativeWindow = w;

    auto sys = InputSystem::Create(cfg);
    ASSERT_TRUE(sys != nullptr);

    sys->pump();
    sys->beginFrame(0.016);
    sys->endFrame();

    glfwDestroyWindow(w);
    glfwTerminate();
}
#endif
