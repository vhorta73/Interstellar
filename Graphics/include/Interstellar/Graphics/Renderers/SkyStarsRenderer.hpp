#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Interstellar/Graphics/IGraphics.hpp"
#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IMaterial.hpp"
#include "Interstellar/Graphics/IShader.hpp"
#include "Interstellar/Engine/Cameras/CameraRig3D.hpp"

namespace Interstellar::Graphics::Renderers {

    class SkyStarsRenderer {
    public:
        explicit SkyStarsRenderer(IGraphics& gfx, int count = 6000, float radiusKm = 5.0e7f);

        void render(IGraphics& gfx,
            const Interstellar::Engine::Cameras::CameraRig3D& cam,
            int viewportW, int viewportH);

        void setRadius(float km) { radiusKm_ = km; }
        void setCount(int n); // regenerates directions

    private:
        void buildDirections(int n);

        std::shared_ptr<IShader>         shader_;
        std::shared_ptr<IRenderPipeline> pipeline_;
        std::shared_ptr<IMaterial>       material_;

        float radiusKm_ = 5.0e7f; // within far plane (1e8)

        std::vector<glm::vec3> dirs_; // unit directions (Fibonacci sphere)
        std::vector<float>     xyz_;  // packed positions per-frame
    };

} // namespace Interstellar::Graphics::Renderers
