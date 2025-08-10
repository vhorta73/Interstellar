#pragma once
#include <optional>
#include <fmt/format.h>

template <class T>
struct fmt::formatter<std::optional<T>> : fmt::formatter<T> {
    template <typename FormatContext>
    auto format(const std::optional<T>& opt, FormatContext& ctx) {
        if (opt) {
            return fmt::formatter<T>::format(*opt, ctx);
        }
        return fmt::format_to(ctx.out(), "null");
    }
};
