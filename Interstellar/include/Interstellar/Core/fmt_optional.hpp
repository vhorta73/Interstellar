// fmt_optional.hpp
#pragma once
#include <optional>
#include <fmt/format.h>

template <class T>
struct fmt::formatter<std::optional<T>> : fmt::formatter<T> {
    // reuse T's parsing/format
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) { return fmt::formatter<T>::parse(ctx); }

    template <typename FormatContext>
    auto format(const std::optional<T>& opt, FormatContext& ctx) {
        if (opt) return fmt::formatter<T>::format(*opt, ctx);
        return fmt::format_to(ctx.out(), "nullopt");
    }
};
