#pragma once
#include <memory>
#include <string>

#include <Interstellar/Input/IKeyboard.hpp>
#include <Interstellar/Input/IMouse.hpp>
#include <Interstellar/Engine/Cameras/CameraRig3D.hpp>
#include <Interstellar/Graphics/Renderers/StarsRenderer.hpp>
#include <Interstellar/Universe/Universe3DRecipe.hpp>
#include <Interstellar/Universe/Seed.hpp>
#include <Interstellar/Graphics/IGraphics.hpp>
#include <Interstellar/Universe/Sector3.hpp>

namespace Interstellar::Scenes {

    /**
     * @ingroup Scenes
     * @brief Self-contained scene that combines a @ref CameraRig3D, a @ref StarsRenderer,
     *        and input handling to produce an interactive procedural universe view.
     *
     * Owns all GPU resources through @ref StarsRenderer (allocated in the constructor).
     * The caller is responsible for driving the @ref update / @ref render cycle each frame
     * in that order, and for ensuring the @p gfx, @p kb, and @p mouse references
     * outlive this object.
     *
     * @since 1.0
     */
    class UniverseScene {
    public:
        /**
         * @ingroup Scenes
         * @brief Construct the scene and allocate all GPU resources.
         * @param kb         [in] IKeyboard  - Keyboard interface; must outlive this scene.
         * @param mouse      [in] IMouse     - Mouse interface; must outlive this scene.
         * @param gfx        [in] IGraphics  - Graphics context used to create GPU resources; must outlive this scene.
         * @param masterSeed [in] Seed64     - Deterministic seed for universe generation.
         * @throws None
         * @complexity O(1) amortised (shader compilation paid once here via StarsRenderer).
         * @thread_safety Not thread-safe; construct on the render thread.
         * @since 1.0
         */
        UniverseScene(Interstellar::Input::IKeyboard& kb,
            Interstellar::Input::IMouse& mouse,
            Interstellar::Graphics::IGraphics& gfx,
            Interstellar::Universe::Seed64 masterSeed);

        /**
         * @ingroup Scenes
         * @brief Process input and advance camera state for the current simulation tick.
         * @details Calls @ref CameraRig3D::handleInput then @ref CameraRig3D::update,
         *          and handles left-click star picking to set the camera focus point.
         * @param viewportW [in] int    - Current framebuffer width in pixels.
         * @param viewportH [in] int    - Current framebuffer height in pixels.
         * @param dt        [in] double - Delta time in seconds since the last tick.
         * @throws None
         * @complexity O(N) on click (star pick); O(1) otherwise.
         * @thread_safety Not thread-safe; call from the simulation thread only.
         * @since 1.0
         */
        void update(int viewportW, int viewportH, double dt);

        /**
         * @ingroup Scenes
         * @brief Render the star field for the current frame.
         * @details Must be called between @ref IGraphics::BeginFrame and
         *          @ref IGraphics::EndFrame.  @ref update must have been called at
         *          least once prior to the first render.
         * @param gfx       [in] IGraphics - Active graphics context.
         * @param viewportW [in] int       - Current framebuffer width in pixels.
         * @param viewportH [in] int       - Current framebuffer height in pixels.
         * @throws None
         * @complexity O(S) where S is the number of universe sectors in the query AABB.
         * @thread_safety Not thread-safe; call from the render thread only.
         * @since 1.0
         */
        void render(Interstellar::Graphics::IGraphics& gfx,
            int viewportW, int viewportH);

        /**
         * @ingroup Scenes
         * @brief Build a detailed single-line HUD string showing world position and sector.
         * @return std::string - Formatted string: @c "pos[km]=(x,y,z) sector=(sx,sy,sz) S=<sectorKm> km".
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        std::string hudLine() const;

        /**
         * @ingroup Scenes
         * @brief Build a compact window-title string showing rounded world position and quadrant.
         * @return std::string - Formatted string: @c "x:<X>  y:<Y>  z:<Z>  quadrant:(qx,qy,qz)".
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        std::string hudTitle() const;

    private:
        Interstellar::Input::IKeyboard& kb_;
        Interstellar::Input::IMouse&    mouse_;

        Interstellar::Engine::Cameras::CameraRig3D                        cam_;
        std::unique_ptr<Interstellar::Graphics::Renderers::StarsRenderer> stars_;

        Interstellar::Universe::Universe3DRecipe recipe_;
        Interstellar::Universe::Seed64           masterSeed_;

        bool   lmbPrev_      = false;
        double hudLogAccum_  = 0.0;
    };

} // namespace Interstellar::Scenes
