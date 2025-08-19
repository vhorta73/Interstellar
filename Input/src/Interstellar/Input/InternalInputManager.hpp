#pragma once
/**
 * @file
 * @brief Extended input manager interface with internal-only features.
 * @ingroup EngineInterfaces
 * @since 1.0
 */

#include "Interstellar/Input/IInputManager.hpp"
#include "Interstellar/Input/KeyCode.hpp"

namespace Interstellar {
    namespace Input {

        /**
         * @brief Extended input manager with engine-only features.
         *
         * This interface extends IInputManager for use inside the engine core.
         * It is not intended for use in gameplay code.
         *
         * @since 1.0
         */
        class InternalInputManager : public IInputManager {
        public:
            virtual ~InternalInputManager() noexcept = default;

            InternalInputManager(const InternalInputManager&) = delete;
            InternalInputManager& operator=(const InternalInputManager&) = delete;
            InternalInputManager(InternalInputManager&&) = delete;
            InternalInputManager& operator=(InternalInputManager&&) = delete;

            /**
             * @brief Injects a synthetic key press into the input system.
             *
             * Used for testing, AI scripting, or simulated user input. The key must
             * be a valid engine-level KeyCode.
             *
             * @param key Engine key code to inject.
             * @since 1.0
             */
            virtual void InjectSyntheticKeyPress(KeyCode key) noexcept = 0;

            /**
             * @brief Reloads input configuration settings at runtime.
             *
             * This allows re-reading key bindings, sensitivity, or platform overrides
             * from configuration files without restarting the game.
             *
             * Must be idempotent and safe to call multiple times.
             *
             * @since 1.0
             * @internal Used only by engine systems, not part of the public interface.
             */
            virtual void ReloadConfig() noexcept = 0;

        protected:
            InternalInputManager() = default;
        };

    }
} // namespace Interstellar::Input
