#pragma once
#include <Interstellar/Input/InputSystem.hpp>
#include "../../Core/InputSystem_Impl.hpp"

namespace Interstellar::Input::Platforms::Null {

    // Internal: Null backend. No OS events; just advances internal states.
    class InputSystem_Null final : public Core::InputSystem_Impl {
    public:
        // Report backend type.
        InputConfig::Backend backend() const override { return InputConfig::Backend::Null; }

        // Construct with config (unused in Null).
        explicit InputSystem_Null(const InputConfig&) {}

        // No-op: no platform events to pump.
        void pump() override {}

        // Advance per-frame device state.
        void beginFrame(double dt) override {
            keyboard_.beginFrame(dt);
            mouse_.beginFrame(dt);
        }

        // No-op: nothing to finalize.
        void endFrame() override {}
    };

} // namespace Interstellar::Input::Platforms::Null
