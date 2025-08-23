#include <Interstellar/Input/InputSystem.hpp>

#include "../Platforms/Null/InputSystem_Null.hpp"

#if defined(ISTELLAR_INPUT_HAVE_GLFW)
#include "../Platforms/GLFW/InputSystem_GLFW.hpp"
#endif

namespace Interstellar::Input {

    std::unique_ptr<InputSystem> InputSystem::Create(const InputConfig& cfg) {
        using B = InputConfig::Backend;

        if (cfg.backend == B::Null) {
            return std::make_unique<Platforms::Null::InputSystem_Null>(cfg);
        }

#if defined(ISTELLAR_INPUT_HAVE_GLFW)
        if (cfg.backend == B::GLFW || cfg.backend == B::Auto) {
            return std::make_unique<Platforms::GLFW::InputSystem_GLFW>(cfg);
        }
#endif

        // Fallback
        return std::make_unique<Platforms::Null::InputSystem_Null>(cfg);
    }

} // namespace Interstellar::Input
