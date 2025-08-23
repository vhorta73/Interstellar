#pragma once
#include <memory>

namespace Interstellar::Input {

    class IKeyboard;
    class IMouse;

    struct InputConfig {
        void* nativeWindow = nullptr;
        enum class Backend { Auto, Null, GLFW } backend = Backend::Auto;
        bool preferPlatformRaw = true;
    };

    /**
     * @ingroup Input
     * @brief Convert a backend enum to a human-readable string.
     * @param b [in] InputConfig::Backend - Backend enum value.
     * @return const char* - "Auto", "Null", "GLFW", or "Unknown".
     * @throws None
     * @complexity O(1)
     * @since 1.0
     */
    constexpr inline const char* BackendName(InputConfig::Backend b) {
        switch (b) {
        case InputConfig::Backend::Auto: return "Auto";
        case InputConfig::Backend::Null: return "Null";
        case InputConfig::Backend::GLFW: return "GLFW";
        default: return "Unknown";
        }
    }

    class InputSystem {
    public:
        /**
         * @ingroup Input
         * @brief Create a new input system instance.
         * @param cfg [in] const InputConfig& - Configuration with backend selection
         *        and native window pointer if required.
         * @return std::unique_ptr<InputSystem> - Owning pointer to a new system.
         * @throws std::bad_alloc - On allocation failure.
         * @since 1.0
         */
        static std::unique_ptr<InputSystem> Create(const InputConfig& cfg);

        /**
         * @ingroup Input
         * @brief Virtual destructor for interface cleanup.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        virtual ~InputSystem() = default;

        /**
         * @ingroup Input
         * @brief Query which backend this instance is actually using.
         * @return InputConfig::Backend - Runtime backend.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        virtual InputConfig::Backend backend() const = 0;

        /**
         * @ingroup Input
         * @brief Pump the underlying platform event queue.
         * @details Collects pending OS events and feeds them to device managers.
         * @throws None
         * @complexity O(N) where N is the number of pending events.
         * @since 1.0
         */
        virtual void pump() = 0;

        /**
         * @ingroup Input
         * @brief Begin a new input frame.
         * @param dtSeconds [in] double - Delta time in seconds.
         * @throws None
         * @pre pump() should be called before this within the same frame.
         * @post State snapshot updated for the frame.
         * @complexity O(1)
         * @since 1.0
         */
        virtual void beginFrame(double dtSeconds) = 0;

        /**
         * @ingroup Input
         * @brief End the current input frame.
         * @throws None
         * @pre beginFrame() was called earlier in the frame.
         * @post Per-frame edge state committed.
         * @complexity O(1)
         * @since 1.0
         */
        virtual void endFrame() = 0;

        /**
         * @ingroup Input
         * @brief Access the system's keyboard interface.
         * @return IKeyboard& - Reference to the keyboard instance.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        virtual IKeyboard& keyboard() = 0;

        /**
         * @ingroup Input
         * @brief Access the system's mouse interface.
         * @return IMouse& - Reference to the mouse instance.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        virtual IMouse& mouse() = 0;
    };

} // namespace Interstellar::Input
