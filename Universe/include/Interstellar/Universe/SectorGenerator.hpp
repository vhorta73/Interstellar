#pragma once
#include <vector>
#include "Interstellar/Universe/RngPCG32.hpp"
#include "Interstellar/Universe/Sector.hpp"
#include "Interstellar/Simulation/Components/Render2D.hpp"
#include "Interstellar/ECS/World.hpp"

namespace Interstellar::Universe {

    struct SectorParams {
        int   count = 200;      // how many points (stars/asteroids) in this sector
        float halfExtent = 512; // +/- extent in sector-local space
    };

    inline void PopulateSector2D(Interstellar::ECS::World& world,
        uint64_t masterSeed,
        SectorCoord sector,
        const SectorParams& params)
    {
        using Interstellar::Simulation::Components::Position2D;
        using Interstellar::Simulation::Components::Colour;

        RngPCG32 rng(deriveSectorSeed(masterSeed, sector), 0x4d595df4d0f33173ULL);

        for (int i = 0; i < params.count; ++i) {
            // random in [-halfExtent, +halfExtent]
            float rx = (rng.nextFloat() * 2.f - 1.f) * params.halfExtent;
            float ry = (rng.nextFloat() * 2.f - 1.f) * params.halfExtent;

            // convert sector-local to world coords (scale sectors by extent*2)
            float worldX = static_cast<float>(sector.x) * (params.halfExtent * 2.f) + rx;
            float worldY = static_cast<float>(sector.y) * (params.halfExtent * 2.f) + ry;

            // spawn in ECS
            auto e = world.create(); // adapt to your World API
            world.assign<Position2D>(e, Position2D{ worldX, worldY });
            world.assign<Colour>(e, Colour{ 1,1,1,1 }); // white; tint later if you want
        }
    }

} // namespace
