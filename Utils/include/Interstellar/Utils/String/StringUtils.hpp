#pragma once
#include <string>

namespace Interstellar::Utils::String {

    static constexpr std::string to_lower_copy(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return s;
    }

    // Lowercase copy, ASCII only (good enough for unit tokens)
    static constexpr std::string to_lower_copy(std::string_view s) {
        std::string out;
        out.resize(s.size());
        for (size_t i = 0; i < s.size(); ++i) {
            unsigned char c = static_cast<unsigned char>(s[i]);
            out[i] = static_cast<char>(std::tolower(c));
        }
        return out;
    }
}
