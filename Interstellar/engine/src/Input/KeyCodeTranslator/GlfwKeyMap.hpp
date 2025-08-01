#pragma once

#include "Interstellar/Input/KeyCodeTranslator/IKeyCodeTranslator.hpp"
#include "Interstellar/Input/KeyCode.hpp"

namespace Interstellar::Input::KeyCodeTranslator {

    /**
     * @ingroup KeyCodes
     * @brief GLFW implementation of key code translation.
     * 
     * @since 1.0
     */
    class GLFWKeyMap : public IKeyCodeTranslator {
    public:
        KeyCode FromPlatform(int glfwKey) const override;
        int ToPlatform(KeyCode code) const override;
    };

}
