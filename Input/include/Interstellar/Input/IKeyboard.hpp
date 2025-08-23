#pragma once
#include "KeyCodes.hpp"

namespace Interstellar::Input {

    class IKeyboard {
    public:
        /**
         * @ingroup Input
         * @brief Virtual destructor for interface cleanup.
         * @throws None
         * @pre None
         * @post None
         * @complexity O(1)
         * @thread_safety Not thread-safe unless an implementation documents otherwise.
         * @reentrancy Yes
         * @since 1.0
         */
        virtual ~IKeyboard() = default;

        /**
         * @ingroup Input
         * @brief Check if a key is currently held down.
         * @param key [in] KeyCode - Logical key.
         * @return bool - true if the key is down for the current frame.
         * @throws None
         * @pre State reflects the current frame.
         * @post None
         * @complexity O(1)
         * @thread_safety Follows implementation; typically not thread-safe.
         * @reentrancy Yes (read-only)
         * @since 1.0
         */
        virtual bool isDown(KeyCode key) const = 0;

        /**
         * @ingroup Input
         * @brief Check if a key transitioned from Up to Down this frame.
         * @param key [in] KeyCode - Logical key.
         * @return bool - true if the key was pressed this frame.
         * @throws None
         * @pre State reflects the current frame.
         * @post None
         * @complexity O(1)
         * @thread_safety Follows implementation; typically not thread-safe.
         * @reentrancy Yes (read-only)
         * @since 1.0
         */
        virtual bool wasPressed(KeyCode key) const = 0;

        /**
         * @ingroup Input
         * @brief Check if a key transitioned from Down to Up this frame.
         * @param key [in] KeyCode - Logical key.
         * @return bool - true if the key was released this frame.
         * @throws None
         * @pre State reflects the current frame.
         * @post None
         * @complexity O(1)
         * @thread_safety Follows implementation; typically not thread-safe.
         * @reentrancy Yes (read-only)
         * @since 1.0
         */
        virtual bool wasReleased(KeyCode key) const = 0;
    };

} // namespace Interstellar::Input
