#pragma once
#include <Interstellar/Units/Physics/Types.hpp>

namespace Interstellar::Units::Physics {

    // Bring symbols into *function* scope (no global pollution in headers)
    constexpr Velocity kmph(Scalar v) {
        using namespace mp_units::si::unit_symbols;
        return v * km / h;  // quantity<kilometre/hour, Scalar>
    }

    constexpr Velocity mps(Scalar v) {
        using namespace mp_units::si::unit_symbols;
        return v * m / s;   // quantity<metre/second, Scalar>
    }
}
