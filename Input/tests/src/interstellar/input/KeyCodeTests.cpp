#include <gtest/gtest.h>
#include "Interstellar/Input/KeyCode.hpp"

namespace interstellar::input {

    using namespace Interstellar::Input;

    TEST(KeyCode_Enum, UnknownIsZero)
    {
        EXPECT_EQ(static_cast<unsigned>(KeyCode::Unknown), 0u);
    }

    TEST(KeyCode_Enum, CountIsSentinelAndGreaterThanLetters)
    {
        EXPECT_GT(static_cast<unsigned>(KeyCode::Count), static_cast<unsigned>(KeyCode::Z));
    }

    TEST(KeyCode_Util, IsModifier)
    {
        using K = KeyCode;
        EXPECT_TRUE(KeyCodeUtil::IsModifier(K::LeftShift));
        EXPECT_TRUE(KeyCodeUtil::IsModifier(K::RightControl));
        EXPECT_TRUE(KeyCodeUtil::IsModifier(K::LeftAlt));
        EXPECT_TRUE(KeyCodeUtil::IsModifier(K::RightSuper));

        EXPECT_FALSE(KeyCodeUtil::IsModifier(K::A));
        EXPECT_FALSE(KeyCodeUtil::IsModifier(K::F1));
        EXPECT_FALSE(KeyCodeUtil::IsModifier(K::Unknown));
    }

    TEST(KeyCode_Util, IsFunctionKey)
    {
        using K = KeyCode;
        EXPECT_TRUE(KeyCodeUtil::IsFunctionKey(K::F1));
        EXPECT_TRUE(KeyCodeUtil::IsFunctionKey(K::F12));
        EXPECT_TRUE(KeyCodeUtil::IsFunctionKey(K::F24));

        EXPECT_FALSE(KeyCodeUtil::IsFunctionKey(K::A));
        EXPECT_FALSE(KeyCodeUtil::IsFunctionKey(K::Num1));
        EXPECT_FALSE(KeyCodeUtil::IsFunctionKey(K::Unknown));
    }

    TEST(KeyCode_Util, IsArrowKey)
    {
        using K = KeyCode;
        EXPECT_TRUE(KeyCodeUtil::IsArrowKey(K::ArrowUp));
        EXPECT_TRUE(KeyCodeUtil::IsArrowKey(K::ArrowDown));
        EXPECT_TRUE(KeyCodeUtil::IsArrowKey(K::ArrowLeft));
        EXPECT_TRUE(KeyCodeUtil::IsArrowKey(K::ArrowRight));

        EXPECT_FALSE(KeyCodeUtil::IsArrowKey(K::Home));
        EXPECT_FALSE(KeyCodeUtil::IsArrowKey(K::A));
    }
}