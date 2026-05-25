#include "Interstellar/Graphics/Renderers/SkyStarsRenderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#include "Interstellar/Renderers/OpenGL/OpenGLGraphics.hpp"
#include "Interstellar/Renderers/OpenGL/GLPointSubmit.hpp"

using namespace Interstellar::Graphics;
using namespace Interstellar::Graphics::Renderers;
using namespace Interstellar::Engine::Cameras;
using Interstellar::Renderers::OpenGL::OpenGLGraphics;

static std::vector<glm::vec3> fibonacciSphere(int n)
{
    std::vector<glm::vec3> v; v.reserve(n);
    const float golden = (1.0f + std::sqrt(5.0f)) * 0.5f;
    const float angle = 2.0f * 3.14159265358979323846f / (golden);
    for (int i = 0; i < n; ++i) {
        float t = (i + 0.5f) / n;
        float y = 1.0f - 2.0f * t;
        float r = std::sqrt(std::max(0.0f, 1.0f - y * y));
        float phi = i * angle;
        v.emplace_back(r * std::cos(phi), y, r * std::sin(phi));
    }
    return v;
}

SkyStarsRenderer::SkyStarsRenderer(IGraphics& gfx, int count, float radiusKm)
    : radiusKm_(radiusKm)
{
    shader_ = gfx.CreateShader("Stars");   // reuse same point shader
    pipeline_ = gfx.CreatePipeline(shader_);
    material_ = pipeline_->CreateMaterial();

    // Very faint tiny pixels
    glm::vec3 tint(1.0f);
    float minB = 0.0025f, maxB = 0.02f;
    float minPx = 1.0f, maxPx = 2.0f;
    material_->Set("u_Tint", &tint, sizeof(tint));
    material_->Set("u_MinBrightness", &minB, sizeof(minB));
    material_->Set("u_MaxBrightness", &maxB, sizeof(maxB));
    material_->Set("u_MinPx", &minPx, sizeof(minPx));
    material_->Set("u_MaxPx", &maxPx, sizeof(maxPx));

    // Brightness windows (unused in this layer but required by shader)
    float nearB = 1e3f, farB = 1e8f;
    material_->Set("u_BrightNear", &nearB, sizeof(nearB));
    material_->Set("u_BrightFar", &farB, sizeof(farB));

    buildDirections(count);
}

void SkyStarsRenderer::setCount(int n) { buildDirections(n); }

void SkyStarsRenderer::buildDirections(int n)
{
    dirs_ = fibonacciSphere(std::max(16, n));
    xyz_.resize(dirs_.size() * 3);
}

void SkyStarsRenderer::render(IGraphics& gfx,
    const CameraRig3D& cam,
    int viewportW, int viewportH)
{
    if (!pipeline_ || !material_ || dirs_.empty()) return;

    const float aspect = (viewportH > 0) ? float(viewportW) / float(viewportH) : 1.0f;
    const glm::mat4 P = cam.proj(aspect);
    const glm::mat4 V = cam.view();
    const glm::mat4 VP = P * V;
    const glm::vec3 eye = cam.eye();

    // Point-size scale for tiny dots
    float uPointScale = (float)viewportH * 0.5f
        / std::tan(glm::radians(cam.fovDeg * 0.5f))
        * 0.75f; // smaller than near layer
    material_->Set("u_VP", &VP, sizeof(VP));
    material_->Set("u_CamPos", &eye, sizeof(eye));
    material_->Set("u_PointScale", &uPointScale, sizeof(uPointScale));

    // Build world positions at a big radius around the eye.
    // (Translation causes negligible drift; orientation rules the view.)
    for (size_t i = 0; i < dirs_.size(); ++i) {
        glm::vec3 p = eye + dirs_[i] * radiusKm_;
        xyz_[i * 3 + 0] = p.x;
        xyz_[i * 3 + 1] = p.y;
        xyz_[i * 3 + 2] = p.z;
    }

    if (auto* ogl = dynamic_cast<OpenGLGraphics*>(&gfx)) {
        Interstellar::Renderers::OpenGL::GLPointSubmit::draw3D(
            *ogl, pipeline_, material_, xyz_.data(),
            static_cast<int>(xyz_.size() / 3));
    }
}
