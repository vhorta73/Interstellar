#pragma once
#include <array>
#include <cstddef>
#include <algorithm>
#include <Interstellar/Input/IKeyboard.hpp>

namespace Interstellar::Input::Core {

    class KeyboardState final : public IKeyboard {
    public:
        KeyboardState() {
            currentDown.fill(false);
            nextDown.fill(false);
            pressed.fill(false);
            released.fill(false);
        }

        // IKeyboard
        bool isDown(KeyCode key) const override {
            return currentDown[idx(key)];
        }
        bool wasPressed(KeyCode key) const override {
            return pressed[idx(key)];
        }
        bool wasReleased(KeyCode key) const override {
            return released[idx(key)];
        }

        // Backend-facing API
        void setKeyDown(KeyCode key, bool down) {
            nextDown[idx(key)] = down;
        }

        void beginFrame(double /*dt*/) {
            // compute transitions from current->next
            for (std::size_t i = 0; i < currentDown.size(); ++i) {
                const bool c = currentDown[i];
                const bool n = nextDown[i];
                pressed[i] = (!c && n);
                released[i] = (c && !n);
                currentDown[i] = n;
            }
        }

    private:
        static constexpr std::size_t idx(KeyCode k) {
            return static_cast<std::size_t>(k);
        }

        std::array<bool, KeyCodeCount()> currentDown;
        std::array<bool, KeyCodeCount()> nextDown;
        std::array<bool, KeyCodeCount()> pressed;
        std::array<bool, KeyCodeCount()> released;
    };

} // namespace Interstellar::Input::Core
