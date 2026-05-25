#pragma once
#include <memory>
#include "Interstellar/Graphics/IGraphics.hpp"
#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IMaterial.hpp"
#include "Interstellar/Graphics/IShader.hpp"
#include "Interstellar/Engine/Cameras/CameraRig3D.hpp"
#include "Interstellar/Universe/Universe3DRecipe.hpp"

namespace Interstellar::Graphics::Renderers {

    /**
     * @ingroup Graphics
     * @brief Renders the analytical galaxy background glow (Milky Way band + bulge).
     *
     * Draws a fullscreen quad and ray-marches the galaxy density function in the fragment
     * shader to produce the integrated starlight glow of billions of unresolved distant
     * stars.  Must be rendered before per-star point-cloud passes so star sprites layer
     * additively on top of the band.
     *
     * @since 1.0
     */
    class GalaxyGlowRenderer {
    public:
        /**
         * @ingroup Graphics
         * @brief Construct and allocate GPU shader/pipeline/material resources.
         * @param gfx [in] IGraphics - Active graphics context; must outlive this renderer.
         * @throws None
         * @complexity O(1) amortised (shader compilation paid once).
         * @thread_safety Not thread-safe.
         * @since 1.0
         */
        explicit GalaxyGlowRenderer(IGraphics& gfx);

        /**
         * @ingroup Graphics
         * @brief Render the galaxy background glow for the current frame.
         * @details Uploads camera and galaxy-shape uniforms then issues a fullscreen quad
         *          draw.  The fragment shader ray-marches 64 steps through the galaxy density
         *          function (@ref GalaxyDiskMultiplier) to compute integrated starlight along
         *          each view ray.
         * @param gfx       [in] IGraphics          - Active graphics context.
         * @param cam       [in] CameraRig3D         - Current camera pose.
         * @param recipe    [in] Universe3DRecipe    - Galaxy shape parameters.
         * @param viewportW [in] int                 - Viewport width in pixels.
         * @param viewportH [in] int                 - Viewport height in pixels.
         * @throws None
         * @complexity O(1) — fixed 64-step ray-march executed entirely on the GPU.
         * @thread_safety Not thread-safe; call from the render thread only.
         * @since 1.0
         */
        void render(IGraphics& gfx,
            const Interstellar::Engine::Cameras::CameraRig3D& cam,
            const Interstellar::Universe::Universe3DRecipe& recipe,
            int viewportW, int viewportH);

        float brightness = 1.2f; ///< Overall brightness multiplier for the galaxy glow.

    private:
        std::shared_ptr<IShader>         shader_;
        std::shared_ptr<IRenderPipeline> pipeline_;
        std::shared_ptr<IMaterial>       material_;
    };

} // namespace Interstellar::Graphics::Renderers
