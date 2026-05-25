#include "Interstellar/Graphics/Renderers/GalaxyGlowRenderer.hpp"
#include "Interstellar/Renderers/OpenGL/GLPointSubmit.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace Interstellar::Graphics;
using namespace Interstellar::Graphics::Renderers;
using namespace Interstellar::Engine::Cameras;
using namespace Interstellar::Universe;

GalaxyGlowRenderer::GalaxyGlowRenderer(IGraphics& gfx)
{
    shader_   = gfx.CreateShader("GalaxyBg");
    pipeline_ = gfx.CreatePipeline(shader_);
    material_ = pipeline_->CreateMaterial();
}

void GalaxyGlowRenderer::render(IGraphics& gfx,
    const CameraRig3D& cam,
    const Universe3DRecipe& recipe,
    int viewportW, int viewportH)
{
    if (!pipeline_ || !material_ || !recipe.useGalaxy) return;

    const float    aspect = (viewportH > 0) ? float(viewportW) / float(viewportH) : 1.0f;
    const glm::mat4 VP    = cam.proj(aspect) * cam.view();
    const glm::mat4 invVP = glm::inverse(VP);
    const glm::vec3 eye   = cam.eye();

    const auto& g = recipe.galaxy;

    // Extract orthonormal basis from the galaxy orientation matrix (column vectors)
    const glm::vec3 galRight  = g.orientation[0];
    const glm::vec3 galFwd    = g.orientation[1];
    const glm::vec3 galNormal = g.orientation[2];

    material_->Set("u_InvVP",         &invVP,          sizeof(invVP));
    material_->Set("u_CamPos",        &eye,             sizeof(eye));
    material_->Set("u_GalCenter",     &g.center,        sizeof(g.center));
    material_->Set("u_GalRight",      &galRight,        sizeof(galRight));
    material_->Set("u_GalFwd",        &galFwd,          sizeof(galFwd));
    material_->Set("u_GalNormal",     &galNormal,       sizeof(galNormal));
    material_->Set("u_RadialScale",   &g.radialScale,   sizeof(float));
    material_->Set("u_VerticalScale", &g.verticalScale, sizeof(float));
    material_->Set("u_CoreRadius",    &g.coreRadius,    sizeof(float));
    material_->Set("u_CoreBoost",     &g.coreBoost,     sizeof(float));
    material_->Set("u_Brightness",    &brightness,      sizeof(float));

    Interstellar::Renderers::OpenGL::GLPointSubmit::drawFullscreenQuad(gfx, pipeline_, material_);
}
