#include <gtest/gtest.h>
#include <Interstellar/Input/KeyCodes.hpp>
#include <Interstellar/Input/MouseButtons.hpp>

using namespace Interstellar::Input;

TEST(Counts, KeyCodeCountNonZero) {
    EXPECT_GT(KeyCodeCount(), 0u);
}

TEST(Counts, MouseButtonCountNonZero) {
    EXPECT_GT(MouseButtonCount(), 0u);
}
