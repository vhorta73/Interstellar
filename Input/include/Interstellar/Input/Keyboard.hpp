#pragma once
#include <Interstellar/Input/IKeyboard.hpp>
#include <Interstellar/Input/KeyCodes.hpp>

namespace Interstellar::Input {

    /**
     * @ingroup Input
     * @brief Check if a key is currently held down.
     * @param k [in] const IKeyboard& - Keyboard interface.
     * @param key [in] KeyCode - Logical key.
     * @return bool - true if key is down for the current frame.
     * @throws None
     * @complexity O(1)
     * @since 1.0
     */
    inline bool Down(const IKeyboard& k, KeyCode key) { return k.isDown(key); }

    /**
     * @ingroup Input
     * @brief Check if a key was pressed this frame.
     * @param k [in] const IKeyboard& - Keyboard interface.
     * @param key [in] KeyCode - Logical key.
     * @return bool - true if key transitioned Up to Down this frame.
     * @throws None
     * @complexity O(1)
     * @since 1.0
     */
    inline bool Pressed(const IKeyboard& k, KeyCode key) { return k.wasPressed(key); }

    /**
     * @ingroup Input
     * @brief Check if a key was released this frame.
     * @param k [in] const IKeyboard& - Keyboard interface.
     * @param key [in] KeyCode - Logical key.
     * @return bool - true if key transitioned Down to Up this frame.
     * @throws None
     * @complexity O(1)
     * @since 1.0
     */
    inline bool Released(const IKeyboard& k, KeyCode key) { return k.wasReleased(key); }

} // namespace Interstellar::Input
