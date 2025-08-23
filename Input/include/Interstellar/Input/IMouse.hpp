#pragma once
#include "MouseButtons.hpp"

namespace Interstellar::Input {

    struct MousePos { float x{ 0 }, y{ 0 }; };
    struct MouseDelta { float dx{ 0 }, dy{ 0 }; };
    struct MouseWheel { float x{ 0 }, y{ 0 }; };

    class IMouse {
    public:
        /**
         * @ingroup Input
         * @brief Virtual destructor for interface cleanup.
         * @throws None
         * @complexity O(1)
         * @thread_safety Not thread-safe unless an implementation documents otherwise.
         * @since 1.0
         */
        virtual ~IMouse() = default;

        /**
         * @ingroup Input
         * @brief Check if a mouse button is currently held down.
         * @param b [in] MouseButton - Logical button.
         * @return bool - true if button is down for the current frame.
         * @throws None
         * @complexity O(1)
         * @thread_safety Follows implementation; typically not thread-safe.
         * @reentrancy Yes (read-only)
         * @since 1.0
         */
        virtual bool isDown(MouseButton b) const = 0;

        /**
         * @ingroup Input
         * @brief Get current mouse cursor position in window coordinates.
         * @return MousePos - Position in pixels relative to the window (origin top-left).
         * @throws None
         * @complexity O(1)
         * @thread_safety Follows implementation.
         * @reentrancy Yes (read-only)
         * @since 1.0
         */
        virtual MousePos position() const = 0;

        /**
         * @ingroup Input
         * @brief Get mouse movement delta since last beginFrame().
         * @return MouseDelta - Relative movement in pixels.
         * @throws None
         * @complexity O(1)
         * @thread_safety Follows implementation.
         * @reentrancy Yes (read-only)
         * @since 1.0
         */
        virtual MouseDelta delta() const = 0;

        /**
         * @ingroup Input
         * @brief Get mouse wheel delta accumulated this frame.
         * @return MouseWheel - Horizontal (x) and vertical (y) wheel deltas.
         * @throws None
         * @complexity O(1)
         * @thread_safety Follows implementation.
         * @reentrancy Yes (read-only)
         * @since 1.0
         */
        virtual MouseWheel wheel() const = 0;

        /**
         * @ingroup Input
         * @brief Enable or disable relative mouse mode.
         * @details When enabled, cursor is hidden/locked and movement is reported as deltas.
         * @param enabled [in] bool - true to enable, false to disable.
         * @throws None
         * @complexity O(1)
         * @thread_safety Not thread-safe; must be called on the thread owning the window/input backend.
         * @since 1.0
         */
        virtual void setRelativeMode(bool enabled) = 0;

        /**
         * @ingroup Input
         * @brief Query whether relative mouse mode is active.
         * @return bool - true if relative mode is enabled.
         * @throws None
         * @complexity O(1)
         * @thread_safety Follows implementation.
         * @reentrancy Yes (read-only)
         * @since 1.0
         */
        virtual bool relativeMode() const = 0;
    };

} // namespace Interstellar::Input
