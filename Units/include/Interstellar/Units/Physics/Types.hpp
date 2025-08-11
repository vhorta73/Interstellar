#pragma once

#include <mp-units/systems/si.h>
#include <Interstellar/Units/Scalar.hpp>

namespace Interstellar::Units::Physics {

    using Scalar = ::Interstellar::Units::Scalar;

    using Length = mp_units::quantity<mp_units::si::unit_symbols::m, Scalar>;
    using Mass = mp_units::quantity<mp_units::si::unit_symbols::kg, Scalar>;
    using Time = mp_units::quantity<mp_units::si::unit_symbols::s, Scalar>;

    using Velocity =
        mp_units::quantity<mp_units::si::unit_symbols::m / mp_units::si::unit_symbols::s, Scalar>;

    using Acceleration =
        mp_units::quantity<mp_units::si::unit_symbols::m /
        (mp_units::si::unit_symbols::s * mp_units::si::unit_symbols::s), Scalar>;

    using Force = mp_units::quantity<mp_units::si::unit_symbols::N, Scalar>;
    using Energy = mp_units::quantity<mp_units::si::unit_symbols::J, Scalar>;
    using Power = mp_units::quantity<mp_units::si::unit_symbols::W, Scalar>;
}
