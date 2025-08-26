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

    struct GLPointSubmit {
        // 3D points from packed XYZ floats (OpenGL concrete backend)
        static void draw3D(OpenGLGraphics& gfx,
            std::shared_ptr<Interstellar::Graphics::IRenderPipeline> pipeline,
            std::shared_ptr<Interstellar::Graphics::IMaterial> material,
            const float* positionsXYZ,
            int count);

        // 3D points from packed XYZ floats (generic graphics; forwards to OpenGL if available)
        static void draw3D(Interstellar::Graphics::IGraphics& gfx,
            const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
            const std::shared_ptr<Interstellar::Graphics::IMaterial>& material,
            const float* positionsXYZ,
            int count);

        static void draw2D(Interstellar::Graphics::IGraphics& gfx,
            const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
            const std::shared_ptr<Interstellar::Graphics::IMaterial>& material,
            const float* xyPacked,
            int count);

        // 2D points from packed XY floats (existing helper)
        static void draw(Interstellar::Graphics::IGraphics& gfx,
            const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
            const std::shared_ptr<Interstellar::Graphics::IMaterial>& material,
            const float* xyPacked,
            int count);
    };

} // namespace Interstellar::Renderers::OpenGL
