#pragma once

#include <memory>

namespace Interstellar::Input {

    class IMouseManager;
    class IKeyboardManager;

    /**
     * @brief Interface for accessing different input device managers.
     *
     * This interface abstracts away input handling (mouse, keyboard, etc.)
     * to support flexible backends and clean separation of concerns.
     *
     * @since 1.0
     */
    class IInputManager {
    public:
        virtual ~IInputManager() = default;

        /**
         * @brief Returns the manager responsible for mouse input.
         * 
         * @return Reference to the mouse manager.
         * @since 1.0
         */
        virtual IMouseManager& GetMouseManager() = 0;

        /**
         * @brief Returns the manager responsible for keyboard input.
         * 
         * @return Reference to the keyboard manager.
         * @since 1.0
         */
        virtual IKeyboardManager& GetKeyboardManager() = 0;

        /**
         * @brief Factory method to create a platform specific input manager.
         *
         * @param window A pointer to a native windowing handle (e.g., GLFWindow*).
         * @return A unique pointer to the created input manager instance.
         * 
         * @note It is the caller's responsibility to cast the `void*` appropriately.
         * @since 1.0
         */
        [[nodiscard]] static std::unique_ptr<IInputManager> Create(void* window);

        /**
        * @brief Updates all input managers (mouse, keyboard, etc).
        *
        * Should be called once per frame.
        *
        * @since 1.0
        */
        virtual void Update() = 0;

    };
}
