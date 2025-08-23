#pragma once

// Quality-of-life helpers for game code.
// All header-only; no CMake changes required.

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <glm/glm.hpp>

#include <Interstellar/Input/InputSystem.hpp>
#include <Interstellar/Input/IKeyboard.hpp>
#include <Interstellar/Input/IMouse.hpp>
#include <Interstellar/Input/KeyCodes.hpp>
#include <Interstellar/Input/MouseButtons.hpp>

namespace Interstellar::Input {

    // ------------------------------------------------------------
    // 1) RAII Frame Guard (ensures pump/beginFrame/endFrame symmetry)
    // ------------------------------------------------------------
    class InputFrameGuard {
        InputSystem& sys_;
    public:
        /**
         * @ingroup Input
         * @brief Begin an input frame with RAII semantics.
         * @details Calls sys.pump() and sys.beginFrame(dtSeconds) in the constructor.
         *          sys.endFrame() is guaranteed in the destructor.
         * @param sys [in] InputSystem& - Owner of the frame lifecycle.
         * @param dtSeconds [in] double - Delta time in seconds (> 0).
         * @throws None
         * @pre The caller must own the update thread for sys.
         * @post An input frame is open until this guard is destroyed.
         * @complexity O(1)
         * @thread_safety Not thread-safe; call on the thread that drives sys.
         * @reentrancy No
         * @since 1.0
         */
        InputFrameGuard(InputSystem& sys, double dtSeconds) : sys_(sys) {
            sys_.pump();
            sys_.beginFrame(dtSeconds);
        }

        /**
         * @ingroup Input
         * @brief End the input frame started by this guard.
         * @details Calls sys.endFrame() once.
         * @throws None
         * @pre A matching frame was begun in the constructor.
         * @post Input frame closed; per-frame edges are committed.
         * @complexity O(1)
         * @thread_safety Not thread-safe; same thread as constructor.
         * @reentrancy No
         * @since 1.0
         */
        ~InputFrameGuard() {
            sys_.endFrame();
        }

        InputFrameGuard(const InputFrameGuard&) = delete;
        InputFrameGuard& operator=(const InputFrameGuard&) = delete;
    };

    // ------------------------------------------------------------
    // 2) Keyboard sugar
    // ------------------------------------------------------------

    /**
     * @ingroup Input
     * @brief Check if a key was pressed this frame.
     * @param k [in] const IKeyboard& - Keyboard source.
     * @param key [in] KeyCode - Logical key.
     * @return bool - true if key transitioned Up to Down this frame.
     * @throws None
     * @pre Call within an active frame; keyboard state up to date.
     * @post None
     * @complexity O(1)
     * @thread_safety Follows IKeyboard; typically not thread-safe.
     * @reentrancy Yes (read-only)
     * @since 1.0
     */
    inline bool pressed(const IKeyboard& k, KeyCode key) { return k.wasPressed(key); }

    /**
     * @ingroup Input
     * @brief Check if a key is currently held down.
     * @param k [in] const IKeyboard& - Keyboard source.
     * @param key [in] KeyCode - Logical key.
     * @return bool - true if key is down.
     * @throws None
     * @pre State reflects the current frame.
     * @post None
     * @complexity O(1)
     * @thread_safety Follows IKeyboard; typically not thread-safe.
     * @reentrancy Yes (read-only)
     * @since 1.0
     */
    inline bool down(const IKeyboard& k, KeyCode key) { return k.isDown(key); }

    /**
     * @ingroup Input
     * @brief Check if a key was released this frame.
     * @param k [in] const IKeyboard& - Keyboard source.
     * @param key [in] KeyCode - Logical key.
     * @return bool - true if key transitioned Down to Up this frame.
     * @throws None
     * @pre State reflects the current frame.
     * @post None
     * @complexity O(1)
     * @thread_safety Follows IKeyboard; typically not thread-safe.
     * @reentrancy Yes (read-only)
     * @since 1.0
     */
    inline bool released(const IKeyboard& k, KeyCode key) { return k.wasReleased(key); }

    // ------------------------------------------------------------
    // 3) Mouse helpers (normalized delta, wheel zoom)
    // ------------------------------------------------------------
    struct MouseHelpers {
        /**
         * @ingroup Input
         * @brief Mouse delta normalized to approximate NDC space.
         * @details Scales pixel delta to the range [-1..+1] across the window.
         * @param m [in] const IMouse& - Mouse source.
         * @param windowW [in] int - Window width in pixels (> 0 to enable normalization).
         * @param windowH [in] int - Window height in pixels (> 0 to enable normalization).
         * @return glm::vec2 - Normalized delta; {0,0} if windowW or windowH <= 0.
         * @throws None
         * @complexity O(1)
         */
        static inline glm::vec2 deltaNorm(const IMouse& m, int windowW, int windowH) {
            const auto d = m.delta();
            if (windowW <= 0 || windowH <= 0) return { 0.f, 0.f };
            return { static_cast<float>(d.dx / windowW * 2.0),
                     static_cast<float>(d.dy / windowH * 2.0) };
        }

        /**
         * @ingroup Input
         * @brief Convert wheel motion to a clamped zoom value.
         * @details Applies a linear step per wheel unit on Y, then clamps.
         * @param m [in] const IMouse& - Mouse source (requires wheel support).
         * @param current [in] float - Current zoom value.
         * @param step [in] float - Zoom change per wheel tick (default 0.5).
         * @param minZ [in] float - Minimum zoom (default 0.1).
         * @param maxZ [in] float - Maximum zoom (default 5000.0).
         * @return float - New clamped zoom value.
         * @throws None
         * @complexity O(1)
         */
        static inline float wheelZoom(const IMouse& m,
            float current,
            float step = 0.5f,
            float minZ = 0.1f,
            float maxZ = 5000.0f) {
            const auto wh = m.wheel();
            if (wh.y == 0.0f && wh.x == 0.0f) return current;
            const float z = current + wh.y * step;
            return std::clamp(z, minZ, maxZ);
        }
    };

    // ------------------------------------------------------------
    // 4) Simple action binding context (tiny mapping layer)
    // ------------------------------------------------------------
    struct Binding {
        std::string action;
        KeyCode key;
    };

    class InputContext {
        std::vector<Binding> bindings_;
    public:
        /**
         * @ingroup Input
         * @brief Bind an action name to a key.
         * @param action [in] std::string - Action identifier.
         * @param key [in] KeyCode - Logical key.
         * @throws std::bad_alloc - On storage growth.
         * @complexity Amortized O(1).
         */
        void bind(std::string action, KeyCode key) {
            bindings_.push_back(Binding{ std::move(action), key });
        }

        /**
         * @ingroup Input
         * @brief Remove all action bindings.
         * @complexity O(N).
         */
        void clear() { bindings_.clear(); }

        /**
         * @ingroup Input
         * @brief Query if any binding for an action was pressed this frame.
         * @param kb [in] const IKeyboard& - Keyboard source.
         * @param action [in] std::string_view - Action identifier.
         * @return bool - true if any bound key was pressed.
         */
        bool actionPressed(const IKeyboard& kb, std::string_view action) const {
            for (const auto& b : bindings_)
                if (b.action == action && kb.wasPressed(b.key))
                    return true;
            return false;
        }

        /**
         * @ingroup Input
         * @brief Query if any binding for an action is held down.
         */
        bool actionDown(const IKeyboard& kb, std::string_view action) const {
            for (const auto& b : bindings_)
                if (b.action == action && kb.isDown(b.key))
                    return true;
            return false;
        }

        /**
         * @ingroup Input
         * @brief Query if any binding for an action was released this frame.
         */
        bool actionReleased(const IKeyboard& kb, std::string_view action) const {
            for (const auto& b : bindings_)
                if (b.action == action && kb.wasReleased(b.key))
                    return true;
            return false;
        }
    };

    // ------------------------------------------------------------
    // 5) Cursor capture RAII (relative mode lock/unlock)
    // ------------------------------------------------------------
    class InputCapture {
        IMouse& mouse_;
        bool prev_{ false };
    public:
        /**
         * @ingroup Input
         * @brief Toggle relative mouse mode for the lifetime of this object.
         * @param mouse [in] IMouse& - Mouse device to control.
         * @param enable [in] bool - Desired relative mode (default true).
         */
        explicit InputCapture(IMouse& mouse, bool enable = true) : mouse_(mouse) {
            prev_ = mouse_.relativeMode();
            mouse_.setRelativeMode(enable);
        }

        /**
         * @ingroup Input
         * @brief Restore the previous relative mouse mode.
         */
        ~InputCapture() { mouse_.setRelativeMode(prev_); }

        InputCapture(const InputCapture&) = delete;
        InputCapture& operator=(const InputCapture&) = delete;
    };

    // ------------------------------------------------------------
    // 6) Mouse smoothing / filtering (exponential)
    // ------------------------------------------------------------
    struct MouseFilter {
        float smoothing = 0.5f;
        glm::vec2 prev{ 0.0f };
        /**
         * @ingroup Input
         * @brief Apply exponential smoothing to normalized mouse delta.
         * @param m [in] const IMouse& - Mouse source.
         * @param windowW [in] int - Window width in pixels.
         * @param windowH [in] int - Window height in pixels.
         * @return glm::vec2 - Smoothed normalized delta.
         */
        glm::vec2 apply(const IMouse& m, int windowW, int windowH) {
            const glm::vec2 n = MouseHelpers::deltaNorm(m, windowW, windowH);
            const float a = std::clamp(1.0f - smoothing, 0.0f, 1.0f);
            prev = prev * (1.0f - a) + n * a;
            return prev;
        }

        /**
         * @ingroup Input
         * @brief Reset the internal smoothing state.
         */
        void reset() { prev = { 0.0f, 0.0f }; }
    };

    // ------------------------------------------------------------
    // 7) Lightweight runtime debug snapshot
    // ------------------------------------------------------------
    struct InputDebug {
        const char* backend = "Unknown";
        bool mouseCaptured = false;
        glm::vec2 mousePos = { 0.0f, 0.0f };
        glm::vec2 mouseDelta = { 0.0f, 0.0f };
    };

    /**
     * @ingroup Input
     * @brief Produce a lightweight debug snapshot of input state.
     * @param sys [in] const InputSystem& - Source system (for backend name).
     * @param m [in] const IMouse& - Mouse device for position/delta.
     * @return InputDebug - Snapshot; valid independently of sys and m.
     */
    inline InputDebug MakeDebug(const InputSystem& sys, const IMouse& m) {
        InputDebug d{};
        d.backend = BackendName(sys.backend());
        d.mouseCaptured = m.relativeMode();
        const auto p = m.position();
        const auto del = m.delta();
        d.mousePos = { p.x, p.y };
        d.mouseDelta = { del.dx, del.dy };
        return d;
    }
} // namespace Interstellar::Input
