#include <Interstellar/Input/Build.hpp>

namespace Interstellar::Input {

    BuildInfo GetBuildInfo() {
        BuildInfo info{};
#if defined(ISTELLAR_INPUT_HAVE_GLFW)
        info.haveGLFW = true;
#endif
        return info;
    }

} // namespace Interstellar::Input
