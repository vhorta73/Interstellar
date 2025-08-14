#pragma once

#include <optional>
#include <fmt/format.h>

namespace fmt {

    // Formatter for std::optional<T> that prints the contained value using T's
    // formatter, or "null" if empty. Supports char and wchar via Char.
    template <typename T, typename Char>
    struct formatter<std::optional<T>, Char> : formatter<T, Char> {
        template <typename FormatContext>
        auto format(const std::optional<T>& opt, FormatContext& ctx) const {
            if (opt) {
                // Use T's formatter with whatever format spec was parsed.
                return formatter<T, Char>::format(*opt, ctx);
            }
            return format_to(ctx.out(), "null");
        }
    };

} // namespace fmt
