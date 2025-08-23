#include <Interstellar/Input/InputSystem.hpp>

#include "Interstellar/Input/Platforms/Null/InputSystem_Null.hpp"

// Select concrete InputSystem by requested backend and compile-time availability.
// Falls back to Null when a requested backend is not available.
#if defined(INTERSTELLAR_INPUT_HAVE_GLFW)
#include "Interstellar/Input/Platforms/GLFW/InputSystem_GLFW.hpp"
#endif

namespace Interstellar::Input {

    std::unique_ptr<InputSystem> InputSystem::Create(const InputConfig& cfg) {
        using B = InputConfig::Backend;

        // Explicit Null request
        if (cfg.backend == B::Null) {
            return std::make_unique<Platforms::Null::InputSystem_Null>(cfg);
        }

#if defined(INTERSTELLAR_INPUT_HAVE_GLFW)
        // If GLFW explicitly requested, construct it (caller must provide a valid window).
        if (cfg.backend == B::GLFW) {
            return std::make_unique<Platforms::GLFW::InputSystem_GLFW>(cfg);
        }

        // Auto: prefer GLFW only if a window pointer is provided; otherwise fall back to Null.
        if (cfg.backend == B::Auto && cfg.nativeWindow != nullptr) {
            return std::make_unique<Platforms::GLFW::InputSystem_GLFW>(cfg);
        }
#endif

        // Fallback
        return std::make_unique<Platforms::Null::InputSystem_Null>(cfg);
    }

} // namespace Interstellar::Input