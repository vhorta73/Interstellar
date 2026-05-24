#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Interstellar/Graphics/IGraphics.hpp"
#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IMaterial.hpp"
#include "Interstellar/Graphics/IShader.hpp"
#include "Interstellar/Engine/Cameras/CameraRig3D.hpp"
#include "Interstellar/Universe/Universe3DRecipe.hpp"
#include "Interstellar/Universe/UniverseQuery.hpp"
#include "Interstellar/Universe/Seed.hpp"
#include "Interstellar/Universe/AABB.hpp"

namespace Interstellar::Graphics::Renderers {

    /**
     * @ingroup Graphics
     * @brief Renders a procedurally-generated star field using two passes:
     *        a near-field deterministic AABB query and a far-shell background impostor.
     *
     * Construct once per graphics context; GPU resources (shader, pipeline, material)
     * are allocated in the constructor.  Call @ref render every frame.  The internal
     * star cache is refreshed each render and is available to @ref pickNearestScreen.
     *
     * @since 1.0
     */
    class StarsRenderer {
    public:
        /**
         * @ingroup Graphics
         * @brief Construct the renderer and allocate GPU shader/pipeline/material resources.
         * @param gfx [in] IGraphics - Active graphics context; must outlive this renderer.
         * @throws None
         * @complexity O(1) amortised (shader compilation paid once here).
         * @thread_safety Not thread-safe; call from the render thread only.
         * @since 1.0
         */
        explicit StarsRenderer(IGraphics& gfx);

        /**
         * @ingroup Graphics
         * @brief Set the near-field AABB query radius in km.
         * @details Stars within this radius are queried deterministically each frame.
         *          Clamped internally to [256, 1e12] km.
         * @param rKm [in] float - Desired query radius in km.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        void setMinQueryRadius(float rKm) { queryRadiusKm_ = rKm; }

        /**
         * @ingroup Graphics
         * @brief Configure the far-shell background pass.
         * @details When enabled, stars beyond the near-field radius are projected onto a
         *          camera-anchored shell so the sky is never empty.  If the far query
         *          yields no stars, a Fibonacci sphere fallback is used automatically.
         * @param enabled  [in] bool  - Enable or disable the background pass.
         * @param radiusKm [in] float - Visual shell radius in km (clamped to ≥1).
         * @param count    [in] int   - Maximum number of background points to render.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        void setBackground(bool enabled, float radiusKm, int count) {
            bgEnabled_  = enabled;
            bgRadiusKm_ = std::max(1.0f, radiusKm);
            bgCount_    = std::max(0, count);
            bgUnitDirs_.clear();
        }

        /**
         * @ingroup Graphics
         * @brief Execute the full two-pass star render for the current frame.
         * @details Pass 1 — near-field AABB query → deterministic point cloud drawn with
         *          depth-based brightness.  Pass 2 (if background enabled) — far-shell
         *          camera-anchored points drawn at a fixed pixel size.
         *          The internal @c lastStars_ cache is refreshed and available to
         *          @ref pickNearestScreen after this call.
         * @param gfx        [in] IGraphics          - Active graphics context.
         * @param cam        [in] CameraRig3D         - Current camera pose and matrices.
         * @param recipe     [in] Universe3DRecipe    - Universe generation parameters.
         * @param masterSeed [in] Seed64              - Deterministic universe seed.
         * @param viewportW  [in] int                 - Viewport width in pixels.
         * @param viewportH  [in] int                 - Viewport height in pixels.
         * @throws None
         * @complexity O(S) where S is the number of sectors touched by the query AABB.
         * @thread_safety Not thread-safe; call from the render thread only.
         * @since 1.0
         */
        void render(IGraphics& gfx,
            const Interstellar::Engine::Cameras::CameraRig3D& cam,
            const Interstellar::Universe::Universe3DRecipe& recipe,
            Interstellar::Universe::Seed64 masterSeed,
            int viewportW, int viewportH);

        /**
         * @ingroup Graphics
         * @brief Result of a screen-space star pick operation.
         * @since 1.0
         */
        struct PickResult {
            std::size_t      index{};    ///< Index into the last near-field star cache.
            glm::vec3        pos{};      ///< World position of the picked star (km).
            float            distance{}; ///< Distance from the camera eye to the star (km).
            Universe::Seed64 id{};       ///< Deterministic identifier of the picked star.
        };

        /**
         * @ingroup Graphics
         * @brief Find the nearest star within @p radiusPx screen pixels of the mouse cursor.
         * @details Operates on the @c lastStars_ cache from the most recent @ref render call.
         *          The effective hit radius is expanded by half the star sprite's projected size.
         * @param cam       [in]  CameraRig3D - Camera used for NDC projection.
         * @param viewportW [in]  int         - Viewport width in pixels.
         * @param viewportH [in]  int         - Viewport height in pixels.
         * @param mouseX    [in]  float       - Mouse X coordinate in window pixels.
         * @param mouseY    [in]  float       - Mouse Y coordinate in window pixels.
         * @param radiusPx  [in]  float       - Search radius in pixels.
         * @param out       [out] PickResult  - Populated with the nearest hit on success.
         * @return bool - true if a star was found within @p radiusPx.
         * @throws None
         * @complexity O(N) where N is the number of near-field stars from the last render.
         * @thread_safety Not thread-safe.
         * @reentrancy Yes (read-only on internal cache after render).
         * @since 1.0
         */
        bool pickNearestScreen(const Engine::Cameras::CameraRig3D& cam,
            int viewportW, int viewportH,
            float mouseX, float mouseY,
            float radiusPx,
            PickResult& out) const;

    private:
        void ensureBackgroundDirs(Interstellar::Universe::Seed64) {}

        std::shared_ptr<IShader>         shader_;
        std::shared_ptr<IRenderPipeline> pipeline_;
        std::shared_ptr<IMaterial>       material_;

        glm::vec3 tint_          = glm::vec3(1.0f);
        float     minBrightness_ = 0.0030f;
        float     maxBrightness_ = 2.25f;
        float     minPx_         = 1.0f;
        float     maxPx_         = 96.0f;

        float     brightNear_    = 5.0e9f;
        float     brightFar_     = 5.0e12f;
        float     baseSizeAtUnit_= 1.0f;

        float queryRadiusKm_     = 1.0e10f;
        static constexpr float kMaxQueryKm = 1.0e12f;

        bool  bgEnabled_  = true;
        float bgRadiusKm_ = 3.0e11f;
        int   bgCount_    = 6000;
        std::vector<glm::vec3> bgUnitDirs_;

        std::vector<float>           starXYZ_;
        std::vector<glm::vec3>       starsWorld_;
        std::vector<Universe::Star3> lastStars_;
        std::vector<glm::vec3>       farShellDirs_;
    };

} // namespace Interstellar::Graphics::Renderers
