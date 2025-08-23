#pragma once
#include <Interstellar/Input/InputSystem.hpp>
#include "../../Core/InputSystem_Impl.hpp"

namespace Interstellar::Input::Platforms::Null {


    class InputSystem_Null final : public Core::InputSystem_Impl {
    public:
        InputConfig::Backend backend() const override { return InputConfig::Backend::Null; }
        explicit InputSystem_Null(const InputConfig&) {}
        void pump() override {}
        void beginFrame(double dt) override { keyboard_.beginFrame(dt); mouse_.beginFrame(dt); }
        void endFrame() override {}
    };

} // namespace Interstellar::Input::Platforms::Null
