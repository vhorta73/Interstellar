#pragma once
#include <array>
#include <cstddef>
#include <Interstellar/Input/IMouse.hpp>

namespace Interstellar::Input::Core {

    class MouseState final : public IMouse {
    public:
        MouseState() {
            currentDown.fill(false);
            nextDown.fill(false);
        }

        // IMouse
        bool isDown(MouseButton b) const override {
            return currentDown[idx(b)];
        }

        MousePos position() const override { return curPos; }
        MouseDelta delta() const override { return { curPos.x - prevPos.x, curPos.y - prevPos.y }; }
        MouseWheel wheel() const override { return wheelDelta; }

        void setRelativeMode(bool e) override { relative = e; }
        bool relativeMode() const override { return relative; }

        // Backend-facing API
        void setButton(MouseButton b, bool down) { nextDown[idx(b)] = down; }
        void setPosition(float x, float y) { nextPos = { x, y }; }
        void addWheel(float x, float y) { wheelAccum.x += x; wheelAccum.y += y; }

        void beginFrame(double /*dt*/) {
            // buttons
            for (std::size_t i = 0; i < currentDown.size(); ++i) {
                currentDown[i] = nextDown[i];
            }
            // position
            prevPos = curPos;
            curPos = nextPos;
         
            // wheel: expose accum for this frame then clear
            wheelDelta = wheelAccum;
            wheelAccum = {};
        }

    private:
        static constexpr std::size_t idx(MouseButton b) {
            return static_cast<std::size_t>(b);
        }

        std::array<bool, MouseButtonCount()> currentDown;
        std::array<bool, MouseButtonCount()> nextDown;

        MousePos curPos{};
        MousePos prevPos{};
        MousePos nextPos{};

        bool relative{ false };

        MouseWheel wheelDelta{}; // what game reads this frame
        MouseWheel wheelAccum{}; // backend adds to this between beginFrame() calls
    };

} // namespace Interstellar::Input::Core
