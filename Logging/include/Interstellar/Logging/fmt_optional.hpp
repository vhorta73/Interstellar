#pragma once
/**
 * @file
 * @brief fmtlib formatter for std::optional<T>.
 *
 * Prints the contained value using T's formatter, or the string "null" if the
 * optional is empty. Works for both narrow and wide formatting (Char == char
 * or wchar_t).
 *
 * @note Do not include this header together with <fmt/std.h> if your fmt version
 *       already provides std::optional support; you'll get a duplicate specialization.
 *       Prefer one approach consistently across the project.
 *
 * @since 1.0
 */

#include <optional>
#include <fmt/format.h>

namespace fmt {

    /**
     * @brief Formatter for std::optional<T> that prints the value or "null".
     * @tparam T    contained type
     * @tparam Char character type (char/wchar_t)
     * @ingroup interstellar_sdk
     *
     * @since 1.0
     */
    template <typename T, typename Char>
    struct formatter<std::optional<T>, Char> : formatter<T, Char> {
        // Provide a clear diagnostic if T isn't formattable with this Char.
        static_assert(fmt::is_formattable<T, Char>::value,
            "fmt::formatter<std::optional<T>> requires T to be formattable");

        template <typename FormatContext>
        auto format(const std::optional<T>& opt, FormatContext& ctx) const {
            if (opt) {
                // Use whatever format spec was parsed by the base formatter<T, Char>.
                return formatter<T, Char>::format(*opt, ctx);
            }
            if constexpr (std::is_same_v<Char, wchar_t>) {
                return format_to(ctx.out(), L"null");
            }
            else {
                return format_to(ctx.out(), "null");
            }
        }
    };

} // namespace fmt
