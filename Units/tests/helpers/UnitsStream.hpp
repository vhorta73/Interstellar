#pragma once
#include <ostream>
#include "Interstellar/Units/Units.hpp"

namespace Interstellar::Units::test_print {

    // Print any temperature-difference quantity as "<value> K"
    template<class Q>
    auto print_kelvin_if_possible(std::ostream& os, const Q& q)
        -> decltype(os << thermo::in_K(q), os)
    {
        return os << thermo::in_K(q) << " K";
    }

} // namespace Interstellar::Units::test_print

// ADL-friendly operator<< that falls back to Kelvin printer if available
template<class Q>
auto operator<<(std::ostream& os, const Q& q)
-> decltype(Interstellar::Units::test_print::print_kelvin_if_possible(os, q))
{
    return Interstellar::Units::test_print::print_kelvin_if_possible(os, q);
}
