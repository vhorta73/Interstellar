#pragma once
#include <array>
#include <cstddef>
#include <Interstellar/Input/IMouse.hpp>

namespace Interstellar::Input::Core {
    /** @cond INTERNAL */
    /**
    * @brief IMouse implementation with double-buffered button state and per-frame wheel accumulation.
    * @details Intended for backend use. Game code should use IMouse via InputSystem.
    */
    class MouseState final : public IMouse {
    public:
        MouseState() {
            currentDown.fill(false);
            nextDown.fill(false);
        }

        // IMouse
        /** @brief True if button is down in the current frame snapshot. */
        bool isDown(MouseButton b) const override { return currentDown[idx(b)]; }

        /** @brief Current cursor position (pixels, window-relative). */
        MousePos position() const override { return curPos; }

        /** @brief Delta since last beginFrame() in pixels. */
        MouseDelta delta() const override { return { curPos.x - prevPos.x, curPos.y - prevPos.y }; }

        /** @brief Wheel delta accumulated for this frame; cleared on beginFrame(). */
        MouseWheel wheel() const override { return wheelDelta; }

        /** @brief Enable or disable relative mode (backend-dependent). */
        void setRelativeMode(bool e) override { relative = e; }

        /** @brief Query relative mode flag. */
        bool relativeMode() const override { return relative; }

        // Backend-facing API
        /** @brief Stage a button state to be committed at next beginFrame(). */
        void setButton(MouseButton b, bool down) { nextDown[idx(b)] = down; }

        /** @brief Stage next cursor position to be committed at next beginFrame(). */
        void setPosition(float x, float y) { nextPos = { x, y }; }

        /** @brief Accumulate raw wheel deltas; exposed for one frame after beginFrame(). */
        void addWheel(float x, float y) { wheelAccum.x += x; wheelAccum.y += y; }

        /**
         * @brief Commit staged input and roll frame buffers.
         * @details Copies nextDown to currentDown, updates prev/cur position,
         *          exposes wheelAccum as wheelDelta, then clears wheelAccum.
         */
        void beginFrame(double /*dt*/) {
            for (std::size_t i = 0; i < currentDown.size(); ++i) { currentDown[i] = nextDown[i]; }
            prevPos = curPos;
            curPos = nextPos;
            wheelDelta = wheelAccum;
            wheelAccum = {};
        }
        /** @endcond */
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
