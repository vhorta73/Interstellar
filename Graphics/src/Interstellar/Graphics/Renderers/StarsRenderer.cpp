#include "Interstellar/Graphics/Renderers/StarsRenderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "Interstellar/Renderers/OpenGL/OpenGLGraphics.hpp"
#include "Interstellar/Renderers/OpenGL/GLPointSubmit.hpp"

using namespace Interstellar::Graphics;
using namespace Interstellar::Graphics::Renderers;
using namespace Interstellar::Engine::Cameras;
using namespace Interstellar::Universe;
using Interstellar::Renderers::OpenGL::OpenGLGraphics;

StarsRenderer::StarsRenderer(IGraphics& gfx)
{
    // Expect assets/shaders/Stars.vert/.frag in your backend
    shader_ = gfx.CreateShader("Stars");
    pipeline_ = gfx.CreatePipeline(shader_);
    material_ = pipeline_->CreateMaterial();

    // Set static uniforms once
    material_->Set("u_Tint", &tint_, sizeof(tint_));
    material_->Set("u_MinBrightness", &minBrightness_, sizeof(minBrightness_));
    material_->Set("u_MaxBrightness", &maxBrightness_, sizeof(maxBrightness_));
    material_->Set("u_MinPx", &minPx_, sizeof(minPx_));
    material_->Set("u_MaxPx", &maxPx_, sizeof(maxPx_));
    material_->Set("u_BrightNear", &brightNear_, sizeof(brightNear_));
    material_->Set("u_BrightFar", &brightFar_, sizeof(brightFar_));
}

void StarsRenderer::render(IGraphics& gfx,
    const CameraRig3D& cam,
    const Universe3DRecipe& recipe,
    Seed64 masterSeed,
    int viewportW, int viewportH)
{
    if (!pipeline_ || !material_) return;

    // ---- Matrices / camera uniforms ----
    const float aspect = (viewportH > 0) ? float(viewportW) / float(viewportH) : 1.0f;
    const glm::mat4 P = cam.proj(aspect);
    const glm::mat4 V = cam.view();
    const glm::mat4 VP = P * V;
    const glm::vec3 eye = cam.eye();

    // Perspective point-size scale: gl_PointSize = clamp(u_PointScale / clip.w, ...)
    float uPointScale = (float)viewportH * 0.5f
        / std::tan(glm::radians(cam.fovDeg * 0.5f))
        * baseSizeAtUnit_;

    material_->Set("u_VP", &VP, sizeof(VP));
    material_->Set("u_CamPos", &eye, sizeof(eye));
    material_->Set("u_PointScale", &uPointScale, sizeof(uPointScale));

    // ---- Build a world-space AABB slab around current camera depth ----
    const float halfH = std::tan(glm::radians(cam.fovDeg * 0.5f)) * cam.z;
    const float halfW = halfH * aspect;
    const float slabHalf = std::max(50.0f, cam.z * 0.08f);

    AABB3 slab3{
        glm::vec3(cam.center.x - halfW, cam.center.y - halfH, cam.z - slabHalf),
        glm::vec3(cam.center.x + halfW, cam.center.y + halfH, cam.z + slabHalf)
    };

    UniverseQueryFilters filters; // stars=true
    UniverseQueryResult  result;
    result.clear();
    QueryUniverseAABB3(masterSeed, recipe, slab3, filters, result);

    // ---- Pack positions as xyz floats ----
    starXYZ_.clear();
    starXYZ_.reserve(result.stars.size() * 3);
    for (const auto& s : result.stars) {
        starXYZ_.push_back(s.pos.x);
        starXYZ_.push_back(s.pos.y);
        starXYZ_.push_back(s.pos.z);
    }

    // ---- Submit as GL points (OpenGL path) ----
    if (auto* ogl = dynamic_cast<OpenGLGraphics*>(&gfx)) {
        Interstellar::Renderers::OpenGL::GLPointSubmit::draw3D(
            *ogl, pipeline_, material_, starXYZ_.data(),
            static_cast<int>(starXYZ_.size() / 3));
    }
    // (You can add a Vulkan path later. For now, silently no-op if not OpenGL.)
}
