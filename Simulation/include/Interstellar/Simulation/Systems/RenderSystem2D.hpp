#pragma once
#include <memory>
#include "Interstellar/Graphics/IMaterial.hpp"
#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IGraphics.hpp"
#include "Interstellar/Simulation/Components/Render2D.hpp"
#include "Interstellar/ECS/World.hpp"

namespace Interstellar::Simulation {

    // Very simple: for each entity with Position2D + Colour: set uniforms and draw.
    inline void RenderSystem2D(Interstellar::ECS::World& world,
        std::shared_ptr<Interstellar::Graphics::IMesh> mesh,
        std::shared_ptr<Interstellar::Graphics::IRenderPipeline> pipeline,
        std::shared_ptr<Interstellar::Graphics::IMaterial> material,
        Interstellar::Graphics::IGraphics* gfx,
        float zoom, float camX, float camY)
    {
        using namespace Interstellar::Simulation::Components;

        // if your shader expects per-draw "u_Zoom" constant:
        material->Set("u_Zoom", &zoom, sizeof(zoom));

        // Example iteration API - adapt to your Worlds query style:
        world.each<Position2D, Colour>([&](auto /*entity*/, const Position2D& pos, const Colour& col) {
            // Compute per-entity offset from camera
            float offset[2] = { pos.x - camX, pos.y - camY };
            material->Set("u_Offset", offset, sizeof(offset));
            material->Set("u_Tint", &col, sizeof(col)); // add this uniform to your shader if you want tinting
            gfx->SubmitMesh(mesh, pipeline, material);
            });
    }

} // namespace
