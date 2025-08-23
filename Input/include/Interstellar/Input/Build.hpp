#pragma once

namespace Interstellar::Input {

    // What the input module was built with (no preprocessor needed in user code)
    struct BuildInfo {
        bool haveGLFW = false;
        // Extend later: haveWin32, haveSDL, version string, git hash, etc.
    };

    /**
     * @ingroup InputBuildInfo
     * @brief Query compile-time feature flags for the Input module.
     * @details Returns a snapshot of features the module was compiled with (e.g., GLFW support).
     *          Use this to branch at runtime without relying on preprocessor defines.
     * @return BuildInfo — Value object describing available integrations and features.
     * @throws None
     * @pre None
     * @post None
     * @complexity O(1)
     * @thread_safety Thread-safe; returns an immutable value object.
     * @reentrancy Yes
     * @since 1.0
     * @see BuildInfo
     */
    BuildInfo GetBuildInfo();

} // namespace Interstellar::Input
