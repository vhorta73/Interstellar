#ifdef INTERSTELLAR_INPUT_HAVE_GLFW
#include <gtest/gtest.h>
#include <GLFW/glfw3.h>
#include <Interstellar/Input/KeyCodes.hpp>
#include <Interstellar/Input/Platforms/GLFW/KeyTranslation_GLFW.hpp>

using namespace Interstellar::Input;
using namespace Interstellar::Input::Platforms::GLFW;

TEST(GLFWTranslation, LettersDigitsFunctionKeys) {
    EXPECT_EQ(TranslateGLFWKey(GLFW_KEY_A), KeyCode::A);
    EXPECT_EQ(TranslateGLFWKey(GLFW_KEY_0), KeyCode::Num0);
    EXPECT_EQ(TranslateGLFWKey(GLFW_KEY_F12), KeyCode::F12);
}

TEST(GLFWTranslation, MouseButtons) {
    EXPECT_EQ(TranslateGLFWMouseButton(GLFW_MOUSE_BUTTON_LEFT), MouseButton::Left);
    EXPECT_EQ(TranslateGLFWMouseButton(GLFW_MOUSE_BUTTON_RIGHT), MouseButton::Right);
}
#endif
