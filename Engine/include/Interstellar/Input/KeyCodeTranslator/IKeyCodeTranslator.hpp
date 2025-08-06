#pragma once

#include "Interstellar/Input/KeyCode.hpp"

namespace Interstellar::Input {

    /**
     * @ingroup KeyCodes
     * @brief Interface for translating platform-specific key codes to engine KeyCodes and back.
     */
    class IKeyCodeTranslator {
    public:
        virtual ~IKeyCodeTranslator() = default;

        /**
         * @brief Converts a platform keycode to engine `KeyCode`.
         * @param platformKey Platform-specific integer key code (e.g., GLFW_KEY_A)
         * @return Equivalent engine-level `KeyCode`.
         */
        virtual KeyCode FromPlatform(int platformKey) const = 0;

        /**
         * @brief Converts an engine `KeyCode` to platform-specific keycode.
         * @param code Engine `KeyCode` enum value.
         * @return Platform-specific integer key code.
         */
        virtual int ToPlatform(KeyCode code) const = 0;
    };

}
