#pragma once
namespace Interstellar::Universe::Defaults3D {
    inline constexpr float SectorSize = 256.0f;     // world units (km if that's your unit)
    inline constexpr float StarDensity = 4.0e-19f;   // stars / unit^3
    inline constexpr float Jitter = 0.35f;
    inline constexpr bool  UseGalaxy = true;

    // Galaxy defaults mirror Galaxy.hpp ctor-ish values
    inline constexpr float GalRadial = 250000.0f;
    inline constexpr float GalVertical = 2000.0f;
    inline constexpr float GalCoreRadius = 15000.0f;
    inline constexpr float GalCoreBoost = 8.0f;
}
