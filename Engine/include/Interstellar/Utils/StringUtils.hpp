#pragma once
#include <string>

namespace Interstellar::Utils {

    inline std::string extractBracketContent(const std::string& input) {
        auto start = input.find('[');
        auto end = input.find(']');

        if (start != std::string::npos && end != std::string::npos && end > start) {
            return input.substr(start + 1, end - start - 1);
        }
        return "";
    }

}
