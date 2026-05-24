#include "Interstellar/Graphics/Renderers/StarsRenderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <limits>
#include <cmath>

#include "Interstellar/Renderers/OpenGL/OpenGLGraphics.hpp"
#include "Interstellar/Renderers/OpenGL/GLPointSubmit.hpp"

using namespace Interstellar::Graphics;
using namespace Interstellar::Graphics::Renderers;
using namespace Interstellar::Engine::Cameras;
using namespace Interstellar::Universe;
using Interstellar::Renderers::OpenGL::OpenGLGraphics;

StarsRenderer::StarsRenderer(IGraphics& gfx)
{
    shader_ = gfx.CreateShader("Stars");
    pipeline_ = gfx.CreatePipeline(shader_);
    material_ = pipeline_->CreateMaterial();

    material_->Set("u_Tint", &tint_, sizeof(tint_));
    material_->Set("u_MinBrightness", &minBrightness_, sizeof(minBrightness_));
    material_->Set("u_MaxBrightness", &maxBrightness_, sizeof(maxBrightness_));
    material_->Set("u_MinPx", &minPx_, sizeof(minPx_));
    material_->Set("u_MaxPx", &maxPx_, sizeof(maxPx_));
    material_->Set("u_BrightNear", &brightNear_, sizeof(brightNear_));
    material_->Set("u_BrightFar", &brightFar_, sizeof(brightFar_));

    int bgFlag = 0;  material_->Set("u_Background", &bgFlag, sizeof(bgFlag));
    float bgSz = 2.0f; material_->Set("u_BGFixedSizePx", &bgSz, sizeof(bgSz));
}

// Deterministically sub-sample by star id so counts cap without flicker.
static inline bool keepStarDeterministically(Seed64 id, double keepProb) {
    if (keepProb >= 1.0) return true;
    if (keepProb <= 0.0) return false;
    constexpr Seed64 PHI = 0x9E3779B97F4A7C15ull;
    Seed64 h = id * PHI ^ (id >> 33);
    uint32_t top = static_cast<uint32_t>(h >> 32);
    double u = static_cast<double>(top) / 4294967296.0;
    return u < keepProb;
}

// Fibonacci sphere fallback for times when the far-shell query yields nothing.
static void buildFibonacciUnitSphere(int n, std::vector<glm::vec3>& out) {
    out.clear(); out.reserve(std::max(0, n));
    if (n <= 0) return;
    const float ga = 2.39996323f; // golden angle
    const float invN = 1.0f / float(n);
    for (int i = 0; i < n; ++i) {
        float y = 1.0f - (2.0f * (i + 0.5f) * invN);
        float r = std::sqrt(std::max(0.0f, 1.0f - y * y));
        float phi = ga * i;
        out.emplace_back(glm::normalize(glm::vec3(std::cos(phi) * r, y, std::sin(phi) * r)));
    }
}

void StarsRenderer::render(IGraphics& gfx,
    const CameraRig3D& cam,
    const Universe3DRecipe& recipe,
    Seed64 masterSeed,
    int viewportW, int viewportH)
{
    if (!pipeline_ || !material_) return;

    const float aspect = (viewportH > 0) ? float(viewportW) / float(viewportH) : 1.0f;
    const glm::mat4 P = cam.proj(aspect);
    const glm::mat4 V = cam.view();
    const glm::mat4 VP = P * V;
    const glm::vec3 eye = cam.eye();

    // Keep brightness windows fixed (don’t tie to FOV)
    material_->Set("u_VP", &VP, sizeof(VP));
    material_->Set("u_CamPos", &eye, sizeof(eye));

    float uPointScale = (float)viewportH * 0.5f
        / std::tan(glm::radians(cam.fovDeg * 0.5f))
        * baseSizeAtUnit_;
    material_->Set("u_PointScale", &uPointScale, sizeof(uPointScale));

    // ---------- Near-field deterministic stars ----------
    float r = std::max(256.0f, std::min(queryRadiusKm_, kMaxQueryKm));
    glm::vec3 r3(r);
    AABB3 aabb{ eye - r3, eye + r3 };

    const float S = std::max(1e-3f, recipe.sectorSize);
    auto cells = [&](float span) { return std::max(1, int(std::ceil(span / S))); };
    int nx = cells(aabb.max.x - aabb.min.x);
    int ny = cells(aabb.max.y - aabb.min.y);
    int nz = cells(aabb.max.z - aabb.min.z);
    long long nsectors = 1ll * nx * ny * nz;

    constexpr long long kMaxSectors = 20000;
    if (nsectors > kMaxSectors) {
        const float shrink = static_cast<float>(
            std::cbrt(static_cast<double>(kMaxSectors) / static_cast<double>(nsectors)));
        r *= std::max(0.40f, shrink);           // don’t collapse to zero
        r3 = glm::vec3(r);
        aabb = { eye - r3, eye + r3 };
    }

    UniverseQueryFilters filters;
    UniverseQueryResult  result; result.clear();
    QueryUniverseAABB3(masterSeed, recipe, aabb, filters, result);
    lastStars_ = result.stars;

    // Submit near stars
    starXYZ_.clear();
    starXYZ_.reserve(result.stars.size() * 3);
    for (const auto& s : result.stars) {
        starXYZ_.push_back(s.pos.x);
        starXYZ_.push_back(s.pos.y);
        starXYZ_.push_back(s.pos.z);
    }
    if (!starXYZ_.empty()) {
        if (auto* ogl = dynamic_cast<OpenGLGraphics*>(&gfx)) {
            Interstellar::Renderers::OpenGL::GLPointSubmit::draw3D(
                *ogl, pipeline_, material_, starXYZ_.data(),
                static_cast<int>(starXYZ_.size() / 3));
        }
    }

    // ---------- Far-shell background from actual stars (camera-anchored) ----------
    if (bgEnabled_ && bgCount_ > 0) {
        const float farLimit = kMaxQueryKm;
        const float rFarVisual = std::min(bgRadiusKm_, farLimit);
        float       rFarQuery = std::min(std::max(rFarVisual * 1.5f, r * 1.25f), kMaxQueryKm);

        farShellDirs_.clear();

        if (rFarQuery > r + 1e-3f) {
            glm::vec3 RQ(rFarQuery);
            AABB3 aabbFar{ eye - RQ, eye + RQ };

            int fnx = cells(aabbFar.max.x - aabbFar.min.x);
            int fny = cells(aabbFar.max.y - aabbFar.min.y);
            int fnz = cells(aabbFar.max.z - aabbFar.min.z);
            long long fsectors = 1ll * fnx * fny * fnz;

            constexpr long long kMaxFarSectors = 30000;
            if (fsectors > kMaxFarSectors) {
                const float shrink = static_cast<float>(
                    std::cbrt(static_cast<double>(kMaxFarSectors) / static_cast<double>(fsectors)));
                rFarQuery *= std::max(0.50f, shrink);
                RQ = glm::vec3(rFarQuery);
                aabbFar = { eye - RQ, eye + RQ };
            }

            UniverseQueryResult farRes; farRes.clear();
            QueryUniverseAABB3(masterSeed, recipe, aabbFar, filters, farRes);

            const int total = static_cast<int>(farRes.stars.size());
            const double keepProb = (total > 0 && total > bgCount_)
                ? double(bgCount_) / double(total)
                : 1.0;

            farShellDirs_.reserve(std::min(bgCount_, total));
            for (const auto& s : farRes.stars) {
                float d = glm::distance(eye, s.pos);
                if (d <= r + 1e-3f) continue;
                if (d > rFarQuery)  continue;
                if (!keepStarDeterministically(s.id, keepProb)) continue;
                glm::vec3 dir = glm::normalize(s.pos - eye);
                if (!glm::any(glm::isnan(dir))) {
                    farShellDirs_.push_back(dir);
                    if ((int)farShellDirs_.size() >= bgCount_) break;
                }
            }
        }

        // Fallback if shell query produced nothing (ensures you never see a blank sky)
        if (farShellDirs_.empty()) {
            buildFibonacciUnitSphere(bgCount_, farShellDirs_);
        }

        // Camera-anchored positions on the visual shell
        starXYZ_.clear();
        starXYZ_.reserve(farShellDirs_.size() * 3);
        for (const auto& d : farShellDirs_) {
            const glm::vec3 p = eye + d * rFarVisual;
            starXYZ_.push_back(p.x);
            starXYZ_.push_back(p.y);
            starXYZ_.push_back(p.z);
        }

        int bgFlag = 1; material_->Set("u_Background", &bgFlag, sizeof(bgFlag));
        float bgSz = 2.0f; material_->Set("u_BGFixedSizePx", &bgSz, sizeof(bgSz));
        if (!starXYZ_.empty()) {
            if (auto* ogl = dynamic_cast<OpenGLGraphics*>(&gfx)) {
                Interstellar::Renderers::OpenGL::GLPointSubmit::draw3D(
                    *ogl, pipeline_, material_, starXYZ_.data(),
                    static_cast<int>(starXYZ_.size() / 3));
            }
        }
        bgFlag = 0; material_->Set("u_Background", &bgFlag, sizeof(bgFlag));
    }
}

bool StarsRenderer::pickNearestScreen(const CameraRig3D& cam,
    int viewportW, int viewportH,
    float mouseX, float mouseY,
    float radiusPx,
    PickResult& out) const
{
    if (lastStars_.empty() || viewportW <= 0 || viewportH <= 0) return false;

    const float aspect = float(viewportW) / float(viewportH);
    const glm::mat4 VP = cam.VP(aspect);
    const glm::vec3 eye = cam.eye();

    const float radius2 = radiusPx * radiusPx;
    float bestD2 = std::numeric_limits<float>::max();
    std::size_t bestIdx = static_cast<std::size_t>(-1);

    const float pointScale = (viewportH * 0.5f)
        / std::tan(glm::radians(cam.fovDeg * 0.5f))
        * baseSizeAtUnit_;

    for (std::size_t i = 0; i < lastStars_.size(); ++i) {
        const glm::vec3& p = lastStars_[i].pos;
        glm::vec4 clip = VP * glm::vec4(p, 1.0f);
        if (clip.w <= 1e-6f) continue;

        glm::vec3 ndc = glm::vec3(clip) / clip.w; // [-1..1]
        float sx = (ndc.x * 0.5f + 0.5f) * viewportW;
        float sy = (1.0f - (ndc.y * 0.5f + 0.5f)) * viewportH;

        float spritePx = glm::clamp(pointScale / clip.w, minPx_, maxPx_);
        float rpick = radiusPx + 0.5f * spritePx;

        float dx = sx - mouseX, dy = sy - mouseY;
        float d2 = dx * dx + dy * dy;
        if (d2 < bestD2 && d2 <= rpick * rpick) { bestD2 = d2; bestIdx = i; }
    }

    if (bestIdx == static_cast<std::size_t>(-1) || bestD2 > radius2) return false;

    const auto& s = lastStars_[bestIdx];
    out.index = bestIdx;
    out.pos = s.pos;
    out.distance = glm::distance(eye, s.pos);
    out.id = s.id;
    return true;
}
