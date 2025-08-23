#include <gtest/gtest.h>
#include <Interstellar/Input/Convenience.hpp>
#include <Interstellar/Input/InputSystem.hpp>

using namespace Interstellar::Input;

namespace {
    struct FakeInputSystem : InputSystem {
        int pumpCalls = 0, beginCalls = 0, endCalls = 0;
        IKeyboard& keyboard() override { return *kb; }
        IMouse& mouse() override { return *ms; }
        InputConfig::Backend backend() const override { return InputConfig::Backend::Null; }
        void pump() override { ++pumpCalls; }
        void beginFrame(double) override { ++beginCalls; }
        void endFrame() override { ++endCalls; }
        std::unique_ptr<IKeyboard> kbStub;
        std::unique_ptr<IMouse> msStub;
        IKeyboard* kb = reinterpret_cast<IKeyboard*>(1);
        IMouse* ms = reinterpret_cast<IMouse*>(1);
        static std::unique_ptr<FakeInputSystem> Make() {
            return std::unique_ptr<FakeInputSystem>(new FakeInputSystem());
        }
    };
}

TEST(FrameGuard, CallsPumpBeginEndExactlyOnce) {
    auto sys = FakeInputSystem::Make();
    {
        InputFrameGuard guard(*sys, 0.016);
        EXPECT_EQ(sys->pumpCalls, 1);
        EXPECT_EQ(sys->beginCalls, 1);
        EXPECT_EQ(sys->endCalls, 0);
    }
    EXPECT_EQ(sys->endCalls, 1);
}
