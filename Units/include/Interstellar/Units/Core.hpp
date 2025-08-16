#pragma once
#include "Interstellar/Units/detail/backend.hpp"

namespace Interstellar::Units {

    // Common type aliases (representative)
    using Length = decltype(1.0 * detail::sym::m);
    using Area = decltype(1.0 * detail::sym::m2);
    using Volume = decltype(1.0 * detail::sym::m3);
    using Time = decltype(1.0 * detail::sym::s);
    using Mass = decltype(1.0 * detail::sym::kg);
    using Amount = decltype(1.0 * detail::sym::mol);
    using Pressure = decltype(1.0 * detail::sym::Pa);
    using Speed = decltype(1.0 * detail::sym::m / detail::sym::s);
    using Acceleration = decltype(1.0 * detail::sym::m / (detail::sym::s * detail::sym::s));
    using Force = decltype(1.0 * detail::sym::N);
    using Energy = decltype(1.0 * detail::sym::J);
    using Power = decltype(1.0 * detail::sym::W);

    // Shared unit-expression catalog (safe, no numbers)
    namespace U {
        inline constexpr auto m = detail::sym::m;
        inline constexpr auto m2 = detail::sym::m2;
        inline constexpr auto m3 = detail::sym::m3;
        inline constexpr auto s = detail::sym::s;
        inline constexpr auto kg = detail::sym::kg;
        inline constexpr auto mol = detail::sym::mol;
        inline constexpr auto Pa = detail::sym::Pa;
        inline constexpr auto m_per_s = detail::sym::m / detail::sym::s;
        inline constexpr auto m_per_s2 = detail::sym::m / (detail::sym::s * detail::sym::s);
        inline constexpr auto N = detail::sym::N;
        inline constexpr auto J = detail::sym::J;
        inline constexpr auto W = detail::sym::W;
    }

    // Scalar extraction (backend-agnostic)
    template<class Q, class UnitExpr>
    inline double in(const Q& q, UnitExpr u) {
        return static_cast<double>(q / (1.0 * u));
    }

} // namespace Interstellar::Units
