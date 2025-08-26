#include "Interstellar/Graphics/renderers/StarsRenderer.hpp"
#include "Interstellar/Renderers/OpenGL/GLPointSubmit.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLGraphics.hpp" // <-- add this

#include <glm/gtc/matrix_transform.hpp>

namespace Interstellar::Graphics {
    using Interstellar::Renderers::OpenGL::GLPointSubmit;

    StarsRenderer::StarsRenderer(IGraphics& gfx) : gfx_(&gfx) {
        auto shader = gfx_->CreateShader("Stars");
        pipeline_ = gfx_->CreatePipeline(shader);
        material_ = pipeline_->CreateMaterial();
        setVisuals(StarVisualParams{}); // defaults
    }

    void StarsRenderer::setVisuals(const StarVisualParams& v) {
        vis_ = v;
        material_->Set("u_Tint", &vis_.tint, sizeof(vis_.tint));
        material_->Set("u_MinBrightness", &vis_.minBrightness, sizeof(vis_.minBrightness));
        material_->Set("u_MaxBrightness", &vis_.maxBrightness, sizeof(vis_.maxBrightness));
        material_->Set("u_MinPx", &vis_.minPx, sizeof(vis_.minPx));
        material_->Set("u_MaxPx", &vis_.maxPx, sizeof(vis_.maxPx));
        material_->Set("u_BrightNear", &vis_.brightNear, sizeof(vis_.brightNear));
        material_->Set("u_BrightFar", &vis_.brightFar, sizeof(vis_.brightFar));
    }

    void StarsRenderer::draw(const std::vector<Interstellar::Universe::Star3>& stars,
        const glm::mat4& VP,
        const glm::vec3& camPos,
        int viewportHeight,
        float fovDeg)
    {
        if (!pipeline_ || !material_) return;

        // Per-frame uniforms
        material_->Set("u_VP", &VP, sizeof(VP));
        material_->Set("u_CamPos", &camPos, sizeof(camPos));

        // gl_PointSize = clamp( u_PointScale / clip.w, u_MinPx, u_MaxPx )
        float uPointScale = float(viewportHeight) * 0.5f
            / std::tan(glm::radians(fovDeg * 0.5f))
            * vis_.baseSizePxAtUnitDepth;
        material_->Set("u_PointScale", &uPointScale, sizeof(uPointScale));

        // Pack XYZ
        xyz_.clear();
        xyz_.reserve(stars.size() * 3);
        for (const auto& s : stars) {
            xyz_.push_back(s.pos.x);
            xyz_.push_back(s.pos.y);
            xyz_.push_back(s.pos.z);
        }

        // Backend-specific submit: GLPointSubmit::draw3D(OpenGLGraphics&, ...)
        using Interstellar::Renderers::OpenGL::OpenGLGraphics;
        auto* ogl = dynamic_cast<OpenGLGraphics*>(gfx_);
        if (!ogl) {
            // Different backend not yet implemented
            return;
        }

        GLPointSubmit::draw3D(*ogl, pipeline_, material_, xyz_.data(), (int)(xyz_.size() / 3));
    }

} // namespace Interstellar::Graphics
