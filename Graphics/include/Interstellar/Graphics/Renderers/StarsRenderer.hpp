#pragma once
#include <memory>
#include <vector>
#include <limits>
#include <glm/glm.hpp>

#include "Interstellar/Graphics/IGraphics.hpp"
#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IMaterial.hpp"
#include "Interstellar/Graphics/IShader.hpp"
#include "Interstellar/Graphics/Renderers/GalaxyGlowRenderer.hpp"
#include "Interstellar/Engine/Cameras/CameraRig3D.hpp"
#include "Interstellar/Universe/Universe3DRecipe.hpp"
#include "Interstellar/Universe/UniverseQuery.hpp"
#include "Interstellar/Universe/Seed.hpp"
#include "Interstellar/Universe/AABB.hpp"

namespace Interstellar::Graphics::Renderers {

    /**
     * @ingroup Graphics
     * @brief Renders a procedurally-generated star field.
     *
     * Each star is drawn with a physically-based angular diameter: apparent pixel size
     * equals @c 2 * radiusKm / distance * focalLengthPx.  Stars shrinking below one
     * pixel fade out smoothly via @c vBright.  No camera-anchored background is used.
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
         * @brief Set the hard visibility cutoff distance in km.
         * @details Stars beyond this distance are discarded in the vertex shader before
         *          any fragment work.  Smaller values improve GPU performance when flying
         *          through dense regions.  Does not affect the AABB query radius; pair with
         *          @ref setMinQueryRadius to keep them consistent.
         * @param km [in] float - Maximum visible distance in km (clamped to ≥1).
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        void setMaxVisibleDistanceKm(float km) { maxVisDistKm_ = std::max(1.0f, km); }

        /**
         * @ingroup Graphics
         * @brief Execute the star render for the current frame.
         * @details Near-field AABB query → deterministic point cloud.  Each star's apparent
         *          pixel size is derived from its physical radius and distance to the camera:
         *          @c sizePx = 2 * radiusKm / dist * focalLengthPx.
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
            int viewportW, int viewportH,
            float nearKm = 1.0f);

        /**
         * @ingroup Graphics
         * @brief Result of a screen-space star pick operation.
         * @since 1.0
         */
        struct PickResult {
            std::size_t      index{};    ///< Index into the last near-field star cache.
            glm::dvec3       pos{};      ///< World position of the picked star (km), double precision.
            float            distance{}; ///< Distance from the camera eye to the star (km).
            Universe::Seed64 id{};       ///< Deterministic identifier of the picked star.
        };

        /**
         * @ingroup Graphics
         * @brief Nearest-star query result from the last render cache.
         * @since 1.0
         */
        struct NearestStar {
            double distKm   = std::numeric_limits<double>::max(); ///< Distance to star centre (km).
            float  radiusKm = 0.f;                                ///< Physical radius of the star (km).
            bool   valid    = false;                              ///< True if the cache is non-empty.
        };

        /**
         * @ingroup Graphics
         * @brief Find the nearest star and return its centre distance and radius.
         * @details Operates on the @c lastStars_ cache populated by the most recent @ref render call.
         *          Use @c distKm - @c radiusKm to obtain altitude above the star surface.
         * @param eye [in] glm::dvec3 - Camera world position (km), double precision.
         * @return NearestStar - Populated result, or default (valid=false) if cache is empty.
         * @throws None
         * @complexity O(N)
         * @since 1.0
         */
        NearestStar nearestStar(const glm::dvec3& eye) const;

        /**
         * @ingroup Graphics
         * @brief Distance in km to the nearest star centre in the last render cache.
         * @param eye [in] glm::dvec3 - Camera world position (km), double precision.
         * @return float - Distance to nearest star centre, or FLT_MAX if cache is empty.
         * @throws None
         * @complexity O(N)
         * @since 1.0
         */
        float nearestDistKm(const glm::dvec3& eye) const;

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
        std::shared_ptr<IShader>         shader_;
        std::shared_ptr<IRenderPipeline> pipeline_;
        std::shared_ptr<IMaterial>       material_;

        std::unique_ptr<GalaxyGlowRenderer> galaxyGlow_;

        glm::vec3 tint_          = glm::vec3(1.0f);
        float     minBrightness_ = 0.0030f;
        float     maxBrightness_ = 2.25f;
        float     minPx_         = 1.0f;
        float     maxPx_         = 16384.0f; // large enough to fill the viewport at stellar surface

        float queryRadiusKm_  = 1.0e10f;
        float maxVisDistKm_   = 5.0e11f;
        static constexpr float kMaxQueryKm = 1.0e12f;

        std::vector<float>           starXYZRI_; // 5 floats per star: x,y,z,radius,intensity
        std::vector<Universe::Star3> lastStars_;
    };

} // namespace Interstellar::Graphics::Renderers
