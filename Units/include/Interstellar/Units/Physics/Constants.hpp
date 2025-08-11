#pragma once

#include <Interstellar/Units/Physics/Types.hpp>

namespace Interstellar::Units::Physics::Constants {

    using namespace mp_units::si::unit_symbols;

    inline constexpr auto G = 6.67430e-11 * m3 / (kg * s2); // gravitational constant
    inline constexpr auto c = 299'792'458 * m / s;          // speed of light
}
