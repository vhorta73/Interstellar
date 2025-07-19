#include "pch.h"                // precompiled header, includes TestTools
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InterstellarMsTests
{
    TEST_CLASS(MyClassTest)
    {
    public:

        TEST_METHOD(TestSeed) { Assert::IsTrue(true); }
    };
};