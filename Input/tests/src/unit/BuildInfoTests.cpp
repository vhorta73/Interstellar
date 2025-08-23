#include <gtest/gtest.h>
#include <Interstellar/Input/Build.hpp>

using namespace Interstellar::Input;

TEST(BuildInfo, GLFWFlagMatchesMacro) {
    BuildInfo bi = GetBuildInfo();
#if defined(INTERSTELLAR_INPUT_HAVE_GLFW)
    EXPECT_TRUE(bi.haveGLFW);
#else
    EXPECT_FALSE(bi.haveGLFW);
#endif
}
