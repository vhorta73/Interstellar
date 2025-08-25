#pragma once
#include <memory>

namespace Interstellar::Graphics {
    class IGraphics;
    class IRenderPipeline;
    class IMaterial;
}

namespace Interstellar::Renderers::OpenGL {

    /**
     * Draws a cloud of points (stars) from packed XY pairs using the given pipeline/material.
     * Expects shader to have:
     *   - layout(location=0) in vec2 a_StarPos;
     *   - uniforms set via IMaterial (e.g., u_VP, u_Zoom, u_StarSize, u_Brightness)
     */
    struct GLPointSubmit {
        static void draw(Interstellar::Graphics::IGraphics& gfx,
            const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
            const std::shared_ptr<Interstellar::Graphics::IMaterial>& material,
            const float* xyPacked, // pointer to [x0,y0, x1,y1, ...]
            int count);            // number of points (pairs)
    };

} // namespace Interstellar::Renderers::OpenGL
