#pragma once
#include <memory>
#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IMaterial.hpp"

namespace Interstellar::Graphics {
    class IGraphics;
    class IRenderPipeline;
    class IMaterial;
}

namespace Interstellar::Renderers::OpenGL {

    class OpenGLGraphics;

    /**
     * @ingroup Renderers
     * @brief Stateless helper for drawing point-cloud geometry via OpenGL.
     *
     * All methods are static; no instance is required.  Shared VAO/VBO buffers
     * are allocated lazily on first use and persist for the lifetime of the process.
     *
     * @since 1.0
     */
    struct GLPointSubmit {

        /**
         * @ingroup Renderers
         * @brief Draw a 3-D point cloud dispatched through the backend-agnostic @ref IGraphics interface.
         * @details Internally casts to @c OpenGLGraphics; silently does nothing if the backend
         *          is not OpenGL.  Prefer this overload in code that should remain backend-neutral.
         * @param gfx          [in] IGraphics             - Active graphics context.
         * @param pipeline     [in] IRenderPipeline shared_ptr - Pipeline owning the shader.
         * @param material     [in] IMaterial shared_ptr    - Material carrying uniform values.
         * @param positionsXYZ [in] const float*            - Packed XYZ positions (3 floats per point).
         * @param count        [in] int                     - Number of points.
         * @throws None
         * @complexity O(N) upload + O(1) draw call.
         * @thread_safety Not thread-safe; call from the render thread only.
         * @since 1.0
         */
        static void draw3D(Interstellar::Graphics::IGraphics& gfx,
            const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
            const std::shared_ptr<Interstellar::Graphics::IMaterial>& material,
            const float* positionsXYZ,
            int count);

        /**
         * @ingroup Renderers
         * @brief Draw a 3-D point cloud where each point carries an extra radius scalar.
         * @details VBO layout per point: [x, y, z, radius] (4 floats, stride 16 bytes).
         *          Binds @c a_Pos (location 0, vec3) and @c a_Radius (location 1, float).
         * @param gfx          [in] IGraphics             - Active graphics context.
         * @param pipeline     [in] IRenderPipeline shared_ptr - Pipeline owning the shader.
         * @param material     [in] IMaterial shared_ptr    - Material carrying uniform values.
         * @param positionsXYZR [in] const float*           - Packed XYZR data (4 floats per point).
         * @param count        [in] int                     - Number of points.
         * @throws None
         * @complexity O(N) upload + O(1) draw call.
         * @thread_safety Not thread-safe; call from the render thread only.
         * @since 1.0
         */
        static void draw3D_xyzr(Interstellar::Graphics::IGraphics& gfx,
            const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
            const std::shared_ptr<Interstellar::Graphics::IMaterial>& material,
            const float* positionsXYZR,
            int count);

        /**
         * @ingroup Renderers
         * @brief Concrete OpenGL overload of @ref draw3D_xyzr.
         * @since 1.0
         */
        static void draw3D_xyzr(OpenGLGraphics& gfx,
            std::shared_ptr<Interstellar::Graphics::IRenderPipeline> pipeline,
            std::shared_ptr<Interstellar::Graphics::IMaterial> material,
            const float* positionsXYZR,
            int count);

        /**
         * @ingroup Renderers
         * @brief Draw a 3-D point cloud directly against the concrete OpenGL backend.
         * @details Use this overload when you already hold an @c OpenGLGraphics reference
         *          and want to avoid the @c dynamic_cast inside the @ref IGraphics overload.
         * @param gfx          [in] OpenGLGraphics            - Concrete OpenGL context.
         * @param pipeline     [in] IRenderPipeline shared_ptr - Pipeline owning the shader.
         * @param material     [in] IMaterial shared_ptr       - Material carrying uniform values.
         * @param positionsXYZ [in] const float*               - Packed XYZ positions (3 floats per point).
         * @param count        [in] int                        - Number of points.
         * @throws None
         * @complexity O(N) upload + O(1) draw call.
         * @thread_safety Not thread-safe; call from the render thread only.
         * @since 1.0
         */
        static void draw3D(OpenGLGraphics& gfx,
            std::shared_ptr<Interstellar::Graphics::IRenderPipeline> pipeline,
            std::shared_ptr<Interstellar::Graphics::IMaterial> material,
            const float* positionsXYZ,
            int count);

        /**
         * @ingroup Renderers
         * @brief Draw a 2-D point cloud (alias for @ref draw).
         * @param gfx      [in] IGraphics             - Active graphics context.
         * @param pipeline [in] IRenderPipeline shared_ptr - Pipeline owning the shader.
         * @param material [in] IMaterial shared_ptr    - Material carrying uniform values.
         * @param xyPacked [in] const float*            - Packed XY positions (2 floats per point).
         * @param count    [in] int                     - Number of points.
         * @throws None
         * @complexity O(N) upload + O(1) draw call.
         * @thread_safety Not thread-safe; call from the render thread only.
         * @since 1.0
         */
        static void draw2D(Interstellar::Graphics::IGraphics& gfx,
            const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
            const std::shared_ptr<Interstellar::Graphics::IMaterial>& material,
            const float* xyPacked,
            int count);

        /**
         * @ingroup Renderers
         * @brief Draw a 2-D point cloud from packed XY float data.
         * @param gfx      [in] IGraphics             - Active graphics context.
         * @param pipeline [in] IRenderPipeline shared_ptr - Pipeline owning the shader.
         * @param material [in] IMaterial shared_ptr    - Material carrying uniform values.
         * @param xyPacked [in] const float*            - Packed XY positions (2 floats per point).
         * @param count    [in] int                     - Number of points.
         * @throws None
         * @complexity O(N) upload + O(1) draw call.
         * @thread_safety Not thread-safe; call from the render thread only.
         * @since 1.0
         */
        static void draw(Interstellar::Graphics::IGraphics& gfx,
            const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
            const std::shared_ptr<Interstellar::Graphics::IMaterial>& material,
            const float* xyPacked,
            int count);

        /**
         * @ingroup Renderers
         * @brief Draw a 3-D point cloud where each point carries position, radius, and intensity.
         * @details VBO layout per point: [x, y, z, radius, intensity] (5 floats, stride 20 bytes).
         *          Binds @c a_Pos (location 0, vec3), @c a_Radius (location 1, float),
         *          @c a_Intensity (location 2, float).
         *          Uses additive blending (@c GL_SRC_ALPHA, @c GL_ONE) so star light accumulates.
         * @param gfx            [in] IGraphics             - Active graphics context.
         * @param pipeline       [in] IRenderPipeline shared_ptr - Pipeline owning the shader.
         * @param material       [in] IMaterial shared_ptr    - Material carrying uniform values.
         * @param positionsXYZRI [in] const float*            - Packed XYZRI data (5 floats per point).
         * @param count          [in] int                     - Number of points.
         * @throws None
         * @complexity O(N) upload + O(1) draw call.
         * @thread_safety Not thread-safe; call from the render thread only.
         * @since 1.0
         */
        static void draw3D_xyzri(Interstellar::Graphics::IGraphics& gfx,
            const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
            const std::shared_ptr<Interstellar::Graphics::IMaterial>& material,
            const float* positionsXYZRI,
            int count);

        /**
         * @ingroup Renderers
         * @brief Render a fullscreen quad (two triangles covering NDC [-1,1]^2).
         * @details Uses normal alpha blending (@c GL_SRC_ALPHA, @c GL_ONE_MINUS_SRC_ALPHA).
         *          Intended for background-pass shaders such as the galaxy glow.
         *          The quad VAO is initialised lazily on first call.
         * @param gfx      [in] IGraphics             - Active graphics context.
         * @param pipeline [in] IRenderPipeline shared_ptr - Pipeline owning the shader.
         * @param material [in] IMaterial shared_ptr    - Material carrying uniform values.
         * @throws None
         * @complexity O(1)
         * @thread_safety Not thread-safe; call from the render thread only.
         * @since 1.0
         */
        static void drawFullscreenQuad(Interstellar::Graphics::IGraphics& gfx,
            const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
            const std::shared_ptr<Interstellar::Graphics::IMaterial>& material);
    };

} // namespace Interstellar::Renderers::OpenGL
