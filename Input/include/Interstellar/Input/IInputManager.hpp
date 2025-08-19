#pragma once
/**
 * @file
 * @brief Aggregates access to device-specific input managers (keyboard, mouse, etc.).
 * @ingroup Devices
 * @ingroup GameInterfaces
 * @since 1.0
 */

#include <memory>

namespace Interstellar::Input {

    class IMouseManager;
    class IKeyboardManager;

    /**
     * @brief Interface for accessing different input device managers.
     *
     * ### Contract
     * - **Threading:** Unless documented otherwise by a concrete implementation,
     *   all functions are expected to be called from the **main thread** (the thread
     *   owning the native window/event loop).
     * - **Lifetime:** References returned by the accessors remain valid for the
     *   lifetime of the `IInputManager` instance.
     * - **Nullability:** Accessors never return null; they return references.
     * - **Update cadence:** Call `Update()` exactly once per frame to advance
     *   device state (edge transitions, deltas, buffering).
     *
     * @since 1.0
     */
    class IInputManager {
    public:
        virtual ~IInputManager() noexcept = default;

        IInputManager(const IInputManager&) = delete;
        IInputManager& operator=(const IInputManager&) = delete;
        IInputManager(IInputManager&&) = delete;
        IInputManager& operator=(IInputManager&&) = delete;

        /**
         * @brief Returns the manager responsible for mouse input.
         * @return Reference to the mouse manager. Non-owning; valid while this input manager is alive.
         * @since 1.0
         */
        [[nodiscard]] virtual IMouseManager& GetMouseManager() noexcept = 0;

        /**
         * @brief Const-qualified access to the mouse manager.
         * @return Reference to the mouse manager.
         * @since 1.0
         */
        [[nodiscard]] virtual const IMouseManager& GetMouseManager() const noexcept = 0;

        /**
         * @brief Returns the manager responsible for keyboard input.
         * @return Reference to the keyboard manager. Non-owning; valid while this input manager is alive.
         * @since 1.0
         */
        [[nodiscard]] virtual IKeyboardManager& GetKeyboardManager() noexcept = 0;

        /**
         * @brief Const-qualified access to the keyboard manager.
         * @return Reference to the keyboard manager.
         * @since 1.0
         */
        [[nodiscard]] virtual const IKeyboardManager& GetKeyboardManager() const noexcept = 0;

        /**
         * @brief Factory method to create a platform-specific input manager.
         *
         * Expected handle types include (but are not limited to) `GLFWwindow*`, `SDL_Window*`, or `HWND`.
         * The handle must remain valid for the lifetime of the created `IInputManager`, unless the
         * concrete implementation states otherwise.
         *
         * @param window Pointer to the native window (may be null for headless/test implementations).
         * @return A unique pointer to the created input manager instance.
         * @note It is the caller's responsibility to cast the `void*` appropriately.
         * @since 1.0
         */
        [[nodiscard]] static std::unique_ptr<IInputManager> Create(void* window) noexcept;

        /**
         * @brief Updates all input managers (mouse, keyboard, etc.).
         *
         * Call exactly once per frame to advance internal state transitions
         * (pressed/released edges, movement deltas, scroll accumulation, etc.).
         *
         * @since 1.0
         */
        virtual void Update() noexcept = 0;

    protected:
        IInputManager() = default;
    };

} // namespace Interstellar::Input
