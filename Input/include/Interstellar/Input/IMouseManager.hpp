#pragma once
/**
 * @file
 * @brief Interface for managing mouse input.
 * @ingroup Devices
 * @ingroup GameInterfaces
 * @since 1.0
 */

#include "Interstellar/Input/MouseButton.hpp"  // enum class MouseButton { Left, Right, Middle, ... }

namespace Interstellar {
    namespace Input {

        /**
         * @brief Manages mouse state queries for gameplay and engine systems.
         *
         * Contract:
         * - Threading: unless documented otherwise by the backend, call from the main
         *   thread that owns the native window/event loop.
         * - Coordinates: returned positions are pixel coordinates in screen space with
         *   origin at the top-left of the window unless the backend says otherwise.
         * - Update cadence: edge queries (WasButtonPressed, WasButtonReleased) advance
         *   on the frame boundary controlled by IInputManager::Update().
         *
         * @since 1.0
         */
        class IMouseManager {
        public:
            virtual ~IMouseManager() noexcept = default;

            IMouseManager(const IMouseManager&) = delete;
            IMouseManager& operator=(const IMouseManager&) = delete;
            IMouseManager(IMouseManager&&) = delete;
            IMouseManager& operator=(IMouseManager&&) = delete;

            /**
             * @brief Returns the current X position in screen coordinates.
             * @return Mouse X position in pixels.
             * @since 1.0
             */
            [[nodiscard]] virtual double GetX() const noexcept = 0;

            /**
             * @brief Returns the current Y position in screen coordinates.
             * @return Mouse Y position in pixels.
             * @since 1.0
             */
            [[nodiscard]] virtual double GetY() const noexcept = 0;

            /**
             * @brief Returns true if the given mouse button is currently held down.
             * @param button Engine-level mouse button.
             * @return true if held; false otherwise.
             * @since 1.0
             */
            [[nodiscard]] virtual bool IsButtonDown(MouseButton button) const noexcept = 0;

            /**
             * @brief Returns true if the given mouse button is currently not held down.
             * @param button Engine-level mouse button.
             * @return true if not held; false otherwise.
             * @note Convenience wrapper over IsButtonDown. Do not override.
             * @since 1.0
             */
            [[nodiscard]] bool IsButtonUp(MouseButton button) const noexcept {
                return !IsButtonDown(button);
            }

            /**
             * @brief Returns true if the button transitioned from up to down this frame.
             * @param button Engine-level mouse button.
             * @return true if pressed this frame; false otherwise.
             * @since 1.0
             */
            [[nodiscard]] virtual bool WasButtonPressed(MouseButton button) const noexcept = 0;

            /**
             * @brief Returns true if the button transitioned from down to up this frame.
             * @param button Engine-level mouse button.
             * @return true if released this frame; false otherwise.
             * @since 1.0
             */
            [[nodiscard]] virtual bool WasButtonReleased(MouseButton button) const noexcept = 0;

            /**
             * @brief Returns the accumulated vertical scroll offset since the last update.
             * Positive values scroll up; negative values scroll down.
             * @return Y-axis scroll offset.
             * @since 1.0
             */
            [[nodiscard]] virtual double GetScrollOffsetY() const noexcept = 0;

            /**
             * @brief Returns the accumulated horizontal scroll offset since the last update.
             * Positive values scroll right; negative values scroll left.
             * @return X-axis scroll offset.
             * @since 1.0
             */
            [[nodiscard]] virtual double GetScrollOffsetX() const noexcept = 0;

            /**
             * @brief Returns true if a drag gesture is currently active.
             * A drag typically means at least one button is down while the mouse moves.
             * Exact policy is backend-defined and should be documented there.
             * @return true if dragging; false otherwise.
             * @since 1.0
             */
            [[nodiscard]] virtual bool IsDragging() const noexcept = 0;

            /// Resets accumulated scroll offsets (called after consuming them).
        virtual void ResetScrollOffsets() = 0;

        protected:
            IMouseManager() = default;
        };

    }
} // namespace Interstellar::Input
