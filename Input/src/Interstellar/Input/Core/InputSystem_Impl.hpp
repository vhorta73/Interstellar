#pragma once
#include <Interstellar/Input/InputSystem.hpp>
#include "KeyboardState.hpp"
#include "MouseState.hpp"

namespace Interstellar::Input::Core {

    class InputSystem_Impl : public InputSystem {
    public:
        IKeyboard& keyboard() override { return keyboard_; }
        IMouse& mouse()    override { return mouse_; }

    protected:
        KeyboardState keyboard_;
        MouseState    mouse_;
    };

} // namespace Interstellar::Input::Core
