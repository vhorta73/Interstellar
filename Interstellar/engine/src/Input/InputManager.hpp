#pragma once

#include "Interstellar/Input/IInputManager.hpp"
#include "Interstellar/Input/IMouseManager.hpp"
#include "Interstellar/Input/IKeyboardManager.hpp"

namespace Interstellar::Input {

    /**
     * @ingroup EngineInterfaces
     * @brief Extended input manager interface with internal-only features.
     *
     * Adds features such as synthetic key injection and configuration reload
     * that are only used within the engine's core systems.
     *
     * @since 1.0
     */
    class InputManager : public IInputManager {
    public:
        virtual ~InputManager() = default;

        /**
         * @brief Returns the mouse manager instance.
         *
         * @return Reference to the active mouse manager.
         * @since 1.0
         */
        IMouseManager& GetMouseManager() override = 0;

        /**
         * @brief Returns the keyboard manager instance.
         *
         * @return Reference to the active keyboard manager.
         * @since 1.0
         */
        IKeyboardManager& GetKeyboardManager() override = 0;

        /**
         * @brief Updates all managed input systems.
         *
         * Should be called once per frame during the main loop.
         * @since 1.0
         */
        void Update() override = 0;

        /**
         * @brief Injects a synthetic key press into the input system.
         *
         * Used for testing, AI scripting, or simulated user input. The key code
         * must match the input backend (e.g., GLFW key enums).
         *
         * @param key The key code to inject.
         * @since 1.0
         */
        virtual void InjectSyntheticKeyPress(int key) = 0;

        /**
         * @brief Reloads input configuration settings at runtime.
         *
         * Useful for re-reading key bindings, sensitivity, or platform overrides
         * from configuration files without restarting the game.
         *
         * @since 1.0
         * @internal Used only by engine systems, not part of the public interface.
         */
        virtual void ReloadConfig() = 0;
    };

}
