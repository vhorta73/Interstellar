#include "Interstellar/Graphics/Renderers/StarsRenderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <limits>
#include <cmath>

#include "Interstellar/Renderers/OpenGL/GLPointSubmit.hpp"

using namespace Interstellar::Graphics;
using namespace Interstellar::Graphics::Renderers;
using namespace Interstellar::Engine::Cameras;
using namespace Interstellar::Universe;

StarsRenderer::StarsRenderer(IGraphics& gfx)
{
    shader_   = gfx.CreateShader("Stars");
    pipeline_ = gfx.CreatePipeline(shader_);
    material_ = pipeline_->CreateMaterial();

    material_->Set("u_MinBrightness", &minBrightness_, sizeof(minBrightness_));
    material_->Set("u_MaxBrightness", &maxBrightness_, sizeof(maxBrightness_));
    material_->Set("u_MinPx",         &minPx_,         sizeof(minPx_));
    material_->Set("u_MaxPx",         &maxPx_,         sizeof(maxPx_));
    material_->Set("u_MaxDistKm",     &maxVisDistKm_,  sizeof(maxVisDistKm_));

    galaxyGlow_ = std::make_unique<GalaxyGlowRenderer>(gfx);
}

void StarsRenderer::render(IGraphics& gfx,
    const CameraRig3D& cam,
    const Universe3DRecipe& recipe,
    Seed64 masterSeed,
    int viewportW, int viewportH,
    float nearKm)
{
    if (!pipeline_ || !material_) return;

    // Galaxy background glow must be drawn first so star sprites layer on top additively
    if (galaxyGlow_)
        galaxyGlow_->render(gfx, cam, recipe, viewportW, viewportH);

    const float aspect = (viewportH > 0) ? float(viewportW) / float(viewportH) : 1.0f;

    // Camera-relative VP: no translation term, star positions are uploaded relative to eye.
    // This eliminates floating-point cancellation when the camera is far from world origin.
    const glm::mat4  VP      = cam.VPcr(aspect, nearKm);
    const glm::dvec3 eyeD    = cam.eyeD();
    const glm::vec3  camRelO = glm::vec3(0.0f); // camera is always at origin in render space

    // Focal length in pixels: converts angular radius (rad) to pixel radius.
    const float focalLengthPx = (viewportH > 0)
        ? float(viewportH) * 0.5f / std::tan(glm::radians(cam.fovDeg * 0.5f))
        : 1.0f;

    material_->Set("u_VP",            &VP,             sizeof(VP));
    material_->Set("u_CamPos",        &camRelO,        sizeof(camRelO));
    material_->Set("u_FocalLengthPx", &focalLengthPx,  sizeof(focalLengthPx));
    material_->Set("u_MaxDistKm",     &maxVisDistKm_,  sizeof(maxVisDistKm_));

    // Clamp query radius so sector count doesn't overflow (one shrink pass).
    float r = std::max(256.0f, std::min(queryRadiusKm_, kMaxQueryKm));
    {
        const float S  = std::max(1e-3f, recipe.sectorSize);
        auto cellsClamped = [&](float span) -> long long {
            double c = std::ceil(static_cast<double>(span) / static_cast<double>(S));
            return static_cast<long long>(std::min(c, 1e8));
        };
        long long nx = cellsClamped(2.0f * r);
        long long nsectors = nx * nx * nx;
        constexpr long long kMaxSectors = 20000;
        if (nsectors > kMaxSectors) {
            r *= std::max(0.40f, static_cast<float>(
                std::cbrt(static_cast<double>(kMaxSectors) / static_cast<double>(nsectors))));
        }
    }

    // Build AABB from the float-cast eye position — sufficient for sector selection.
    const glm::vec3 eyeF = glm::vec3(eyeD);
    const glm::vec3 r3(r);
    const AABB3 aabb{ eyeF - r3, eyeF + r3 };

    UniverseQueryFilters filters;
    UniverseQueryResult  result;
    result.clear();
    QueryUniverseAABB3(masterSeed, recipe, aabb, filters, result);
    lastStars_ = result.stars;

    // Pack as XYZRI (5 floats per star).
    // Positions are camera-relative (subtract eyeD in double, then cast to float) so the
    // GPU receives full-precision local coordinates regardless of world-space magnitude.
    starXYZRI_.clear();
    starXYZRI_.reserve(result.stars.size() * 5);
    for (const auto& s : result.stars) {
        starXYZRI_.push_back(float(s.pos.x - eyeD.x));
        starXYZRI_.push_back(float(s.pos.y - eyeD.y));
        starXYZRI_.push_back(float(s.pos.z - eyeD.z));
        starXYZRI_.push_back(s.radiusKm);
        starXYZRI_.push_back(s.intensity);
    }

    if (!starXYZRI_.empty()) {
        Interstellar::Renderers::OpenGL::GLPointSubmit::draw3D_xyzri(
            gfx, pipeline_, material_, starXYZRI_.data(),
            static_cast<int>(starXYZRI_.size() / 5));
    }
}

StarsRenderer::NearestStar StarsRenderer::nearestStar(const glm::dvec3& eye) const
{
    NearestStar best;
    for (const auto& s : lastStars_) {
        double d = glm::length(s.pos - eye);
        if (d < best.distKm) {
            best.distKm   = d;
            best.radiusKm = s.radiusKm;
            best.valid    = true;
        }
    }
    return best;
}

float StarsRenderer::nearestDistKm(const glm::dvec3& eye) const
{
    auto ns = nearestStar(eye);
    return ns.valid ? float(ns.distKm) : std::numeric_limits<float>::max();
}

bool StarsRenderer::pickNearestScreen(const CameraRig3D& cam,
    int viewportW, int viewportH,
    float mouseX, float mouseY,
    float radiusPx,
    PickResult& out) const
{
    if (lastStars_.empty() || viewportW <= 0 || viewportH <= 0) return false;

    const float      aspect  = float(viewportW) / float(viewportH);
    const glm::dvec3 eyeD    = cam.eyeD();
    const glm::mat4  VP      = cam.VPcr(aspect);  // camera-relative VP

    const float radius2 = radiusPx * radiusPx;
    float bestD2 = std::numeric_limits<float>::max();
    std::size_t bestIdx = static_cast<std::size_t>(-1);

    const float focalLengthPx = float(viewportH) * 0.5f
        / std::tan(glm::radians(cam.fovDeg * 0.5f));

    for (std::size_t i = 0; i < lastStars_.size(); ++i) {
        // Camera-relative position for correct projection
        glm::vec3 relPos = glm::vec3(lastStars_[i].pos - eyeD);
        glm::vec4 clip = VP * glm::vec4(relPos, 1.0f);
        if (clip.w <= 1e-6f) continue;

        glm::vec3 ndc = glm::vec3(clip) / clip.w;
        float sx = (ndc.x * 0.5f + 0.5f) * viewportW;
        float sy = (1.0f - (ndc.y * 0.5f + 0.5f)) * viewportH;

        const float dist = float(glm::length(lastStars_[i].pos - eyeD));
        float spritePx = glm::clamp(
            2.0f * lastStars_[i].radiusKm / std::max(dist, 1e-4f) * focalLengthPx,
            minPx_, maxPx_);
        float rpick = radiusPx + 0.5f * spritePx;

        float dx = sx - mouseX, dy = sy - mouseY;
        float d2 = dx * dx + dy * dy;
        if (d2 < bestD2 && d2 <= rpick * rpick) { bestD2 = d2; bestIdx = i; }
    }

    if (bestIdx == static_cast<std::size_t>(-1) || bestD2 > radius2) return false;

    const auto& s = lastStars_[bestIdx];
    out.index    = bestIdx;
    out.pos      = s.pos;
    out.distance = float(glm::length(s.pos - eyeD));
    out.id       = s.id;
    return true;
}
