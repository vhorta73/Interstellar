#pragma once
/**
 * @file
 * @brief GLFW implementation of key code translation.
 * @ingroup InputTranslators
 * @since 1.0
 */

#include "Interstellar/Input/KeyCodeTranslator/IKeyCodeTranslator.hpp"
#include "Interstellar/Input/KeyCode.hpp"

namespace Interstellar {
    namespace Input {
        namespace KeyCodeTranslator {

            /**
             * @brief GLFW-backed translator between platform key codes and engine KeyCode.
             * @since 1.0
             */
            class GLFWKeyMap : public IKeyCodeTranslator {
            public:
                [[nodiscard]] KeyCode FromPlatform(int glfwKey) const noexcept override;
                [[nodiscard]] int ToPlatform(KeyCode code) const noexcept override;
            };

        }
    }
} // namespace Interstellar::Input::KeyCodeTranslator
