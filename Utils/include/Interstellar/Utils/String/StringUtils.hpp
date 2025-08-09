#pragma once
#include <string>

namespace Interstellar::Utils::String {

    inline constexpr std::string to_lower_copy(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return s;
    }

}
