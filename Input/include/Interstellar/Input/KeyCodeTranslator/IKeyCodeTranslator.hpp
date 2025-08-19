#pragma once
/**
 * @file
 * @brief Interface for translating platform-specific key codes to engine `KeyCode` and back.
 * @since 1.0
 */

#include <cstdint>
#include "Interstellar/Input/KeyCode.hpp"

namespace Interstellar::Input {

    /// Platform key type alias (adjust per backend needs).
    using PlatformKey = std::int32_t;

    /**
     * @defgroup Input Input System
     * @{
     */

     /**
      * @defgroup InputTranslators Key Code Translators
      * @ingroup Input
      * @brief Components that map platform key identifiers to engine `KeyCode` values.
      * @{
      */

      /**
       * @brief Interface for translating platform-specific key codes to engine `KeyCode` and back.
       *
       * ### Contract
       * - Implementations must be **re-entrant** and **thread-safe**.
       * - If a mapping does not exist:
       *   - `FromPlatform(...)` returns `KeyCode::Unknown`.
       *   - `ToPlatform(...)` returns `-1` (invalid platform key).
       *
       * @since 1.0
       */
    class IKeyCodeTranslator {
    public:
        virtual ~IKeyCodeTranslator() noexcept = default;

        /**
         * @brief Converts a platform key to an engine `KeyCode`.
         * @param platformKey Platform-specific key identifier (e.g., GLFW key).
         * @return Engine-level `KeyCode`, or `KeyCode::Unknown` if unmapped.
         * @since 1.0
         */
        [[nodiscard]] virtual KeyCode FromPlatform(PlatformKey platformKey) const noexcept = 0;

        /**
         * @brief Converts an engine `KeyCode` to a platform key.
         * @param code Engine `KeyCode` enum value.
         * @return Platform key identifier, or `-1` if unmapped.
         * @since 1.0
         */
        [[nodiscard]] virtual PlatformKey ToPlatform(KeyCode code) const noexcept = 0;
    };

    /** @} */ // end of InputTranslators
    /** @} */ // end of Input

} // namespace Interstellar::Input
