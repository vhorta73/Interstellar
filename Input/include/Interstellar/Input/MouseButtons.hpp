#pragma once
#include <cstdint>

namespace Interstellar::Input {

    enum class MouseButton : uint8_t {
        Left = 0,
        Right,
        Middle,
        // reserve slots for extra buttons if you add later:
        Button4,
        Button5,
        _Count
    };

    /**
     * @ingroup Input
     * @brief Get the number of defined mouse buttons.
     * @return std::size_t - Count of MouseButton values.
     * @throws None
     * @complexity O(1)
     * @since 1.0
     */
    constexpr inline std::size_t MouseButtonCount() {
        return static_cast<std::size_t>(MouseButton::_Count);
    }

} // namespace Interstellar::Input
