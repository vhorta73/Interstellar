#pragma once
#include <memory>

namespace Interstellar::Graphics {
    class IGraphics;
    class IMesh;
    class IRenderPipeline;
    class IMaterial;
}

namespace Interstellar::Renderers::OpenGL {

    // Simple helper to draw an IMesh as many instances using per-instance XY offsets.
    struct GLInstancedSubmit {
        static void draw(Interstellar::Graphics::IGraphics& gfx,
            const std::shared_ptr<Interstellar::Graphics::IMesh>& mesh,
            const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
            const std::shared_ptr<Interstellar::Graphics::IMaterial>& material,
            const float* instanceXY,    // flattened [x0,y0,x1,y1,...]
            int instanceCount);         // number of instances
    };

} // namespace Interstellar::Renderers::OpenGL
