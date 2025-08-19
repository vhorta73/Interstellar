#pragma once
/**
 * @file
 * @brief Interface for managing keyboard input.
 * @ingroup Devices
 * @ingroup GameInterfaces
 * @since 1.0
 */

#include "Interstellar/Input/KeyCode.hpp"

namespace Interstellar {
    namespace Input {

        /**
         * @brief Manages keyboard state queries for gameplay and engine systems.
         *
         * Contract:
         * - Threading: unless documented otherwise by the backend, call from the main
         *   thread that owns the native window/event loop.
         * - Key domain: all queries use engine-level KeyCode values. Unrecognized or
         *   unmapped keys should be KeyCode::Unknown and return false.
         * - Update cadence: edge queries (WasKeyPressed, WasKeyReleased) advance on the
         *   frame boundary controlled by IInputManager::Update().
         *
         * @since 1.0
         */
        class IKeyboardManager {
        public:
            virtual ~IKeyboardManager() noexcept = default;

            IKeyboardManager(const IKeyboardManager&) = delete;
            IKeyboardManager& operator=(const IKeyboardManager&) = delete;
            IKeyboardManager(IKeyboardManager&&) = delete;
            IKeyboardManager& operator=(IKeyboardManager&&) = delete;

            /**
             * @brief Returns true if the key is currently held down.
             * @param key Engine-level key identifier.
             * @return true if held; false otherwise (including KeyCode::Unknown).
             * @since 1.0
             */
            [[nodiscard]] virtual bool IsKeyDown(KeyCode key) const noexcept = 0;

            /**
             * @brief Returns true if the key is currently not held down.
             * @param key Engine-level key identifier.
             * @return true if not held; false otherwise (including KeyCode::Unknown).
             * @note Convenience wrapper over IsKeyDown. Do not override.
             * @since 1.0
             */
            [[nodiscard]] bool IsKeyUp(KeyCode key) const noexcept {
                return !IsKeyDown(key);
            }

            /**
             * @brief Returns true if the key transitioned from up to down this frame.
             * @param key Engine-level key identifier.
             * @return true if pressed this frame; false otherwise.
             * @since 1.0
             */
            [[nodiscard]] virtual bool WasKeyPressed(KeyCode key) const noexcept = 0;

            /**
             * @brief Returns true if the key transitioned from down to up this frame.
             * @param key Engine-level key identifier.
             * @return true if released this frame; false otherwise.
             * @since 1.0
             */
            [[nodiscard]] virtual bool WasKeyReleased(KeyCode key) const noexcept = 0;

        protected:
            IKeyboardManager() = default;
        };

    }
} // namespace Interstellar::Input
