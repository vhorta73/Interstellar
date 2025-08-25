#pragma once
#include <cstdint>

namespace Interstellar::Simulation::Components {

    // world-space position (2D for the triangle demo)
    struct Position2D { float x{}, y{}; };

    // colour tint (RGBA 0..1)
    struct Colour { float r{ 1 }, g{ 1 }, b{ 1 }, a{ 1 }; };

} // namespace
