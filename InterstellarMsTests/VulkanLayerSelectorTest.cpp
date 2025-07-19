#include "pch.h"                // precompiled header, includes TestTools
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InterstellarMsTests {
    TEST_CLASS(InterstellarGraphicsVulkan)
    {
    public:
        TEST_METHOD(TestSeed)
        {
            Assert::AreEqual(7, 7);
            Assert::AreNotEqual(7, 8);
        }
    };
}