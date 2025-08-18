#pragma once
// Tiny shim: prefer std::expected (C++23); otherwise, a small fallback.
// Public API (namespace Interstellar::IO):
//   - expected<T, E>
//   - expected<void, E>
//   - unexpected<E>
//   - make_unexpected(E&&)
//   - unexpect_t / unexpect
//
// Notes:
//   * unexpected<void> is NOT supported.
//   * This is a minimal, header-only shim intended for internal use.
//   * Exception policy: this shim never throws for logic errors; precondition
//     violations in value()/error() trigger an assert.

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
#include <expected>

/// \file Expected.hpp
/// \brief Aliases to C++23 `std::expected` with identical surface where available.
/// \details
///  - When `std::expected` is available, this header aliases the standard types.
///  - When not, a small fallback with a subset of the standard interface is provided.
///  - Exception policy for fallback: `value()`/`error()` assert on misuse (no throws).

namespace Interstellar::IO {
    template<class T, class E> using expected = std::expected<T, E>;
    template<class E>         using unexpected = std::unexpected<E>;
    using std::make_unexpected;
    using std::unexpect_t;
    inline constexpr std::unexpect_t unexpect{};
} // namespace Interstellar::IO

#else // --------------------------- fallback shim ---------------------------

#include <new>          // placement new, std::launder
#include <utility>      // move, forward, addressof
#include <type_traits>  // traits
#include <cassert>      // assert
#include <cstddef>      // size_t

/// \file Expected.hpp
/// \brief Minimal `std::expected`-like types for environments without C++23.
/// \details
///  - Recovers a practical subset of `std::expected`.
///  - No exceptions are thrown by this shim; misuse is caught via `assert`.
///  - Monadic helpers (`and_then`, `transform`, `or_else`) are provided in a lightweight form.
///  - Designed for internal use within Interstellar IO.

namespace Interstellar::IO {

    // ---------------- tag types ----------------

    /// \brief Tag type signaling construction of the error alternative.
    struct unexpect_t { explicit unexpect_t() = default; };
    inline constexpr unexpect_t unexpect{};

    /// \brief Tag type signaling in-place value construction.
    struct in_place_t { explicit in_place_t() = default; };
    inline constexpr in_place_t in_place{};

    // ---------------- unexpected<E> ----------------

    /**
     * @brief Holder for an error value used to construct an `expected` in its error state.
     * @tparam E error type (must not be void).
     * @note Matches the spirit of `std::unexpected`.
     */
    template<class E>
    class unexpected {
        static_assert(!std::is_void_v<E>, "unexpected<void> is not supported.");
    public:
        using error_type = E;

        /// \brief Construct from an error (explicit to avoid accidental conversions).
        explicit unexpected(const E& e) : e_(e) {}
        explicit unexpected(E&& e) noexcept(std::is_nothrow_move_constructible_v<E>) : e_(std::move(e)) {}

        // Observers
        [[nodiscard]] const E& error() const& noexcept { return e_; }
        [[nodiscard]] E& error() & noexcept { return e_; }
        [[nodiscard]] E        error() && noexcept(std::is_nothrow_move_constructible_v<E>) { return std::move(e_); }

        // Modifiers
        void swap(unexpected& other) noexcept(std::is_nothrow_swappable_v<E>) { using std::swap; swap(e_, other.e_); }

        // Comparisons (useful in tests)
        friend bool operator==(const unexpected& a, const unexpected& b) { return a.e_ == b.e_; }
        friend bool operator!=(const unexpected& a, const unexpected& b) { return !(a == b); }

    private:
        E e_;
    };

    /// \brief Factory to avoid global `::unexpected` clashes on some libstdc++/MSVC setups.
    template<class Err>
    [[nodiscard]] unexpected<std::decay_t<Err>> make_unexpected(Err&& e) {
        return unexpected<std::decay_t<Err>>(std::forward<Err>(e));
    }

    // ---------------- expected<T,E> (primary) ----------------

    /**
     * @brief Discriminated union of either a value `T` or an error `E`.
     * @tparam T value type (not void in this primary template).
     * @tparam E error type (must not be void).
     * @details
     * - No exceptions thrown; `value()`/`error()` assert on wrong alternative.
     * - Provides a minimal subset of the C++23 `std::expected` interface.
     */
    template<class T, class E>
    class expected {
        static_assert(!std::is_void_v<T>, "expected<void, E> has a specialization.");
        static_assert(!std::is_void_v<E>, "expected<T, void> is not supported.");
    public:
        using value_type = T;
        using error_type = E;

        // ---------------- constructors: success state ----------------
        expected() noexcept(std::is_nothrow_default_constructible_v<T>)
            requires std::is_default_constructible_v<T>
        : has_(true) {
            ::new (std::addressof(u_.v_)) T();
        }

        expected(const T& v) : has_(true) { ::new (std::addressof(u_.v_)) T(v); }
        expected(T&& v) noexcept(std::is_nothrow_move_constructible_v<T>) : has_(true) {
            ::new (std::addressof(u_.v_)) T(std::move(v));
        }

        template<class... Args>
        explicit expected(in_place_t, Args&&... args)
            : has_(true) {
            ::new (std::addressof(u_.v_)) T(std::forward<Args>(args)...);
        }

        // ---------------- constructors: error state ----------------
        // Intentionally non-explicit: allows `return unexpected<E>{...};`
        expected(unexpected<E> ue) : has_(false) { ::new (std::addressof(u_.e_)) E(std::move(ue.error())); }

        template<class... Args>
        explicit expected(unexpect_t, Args&&... args) : has_(false) {
            ::new (std::addressof(u_.e_)) E(std::forward<Args>(args)...);
        }

        // ---------------- copy/move ----------------
        expected(const expected& o) : has_(o.has_) {
            if (has_) ::new (std::addressof(u_.v_)) T(o.u_.v_);
            else      ::new (std::addressof(u_.e_)) E(o.u_.e_);
        }
        expected(expected&& o) noexcept(
            std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_constructible_v<E>)
            : has_(o.has_) {
            if (has_) ::new (std::addressof(u_.v_)) T(std::move(o.u_.v_));
            else      ::new (std::addressof(u_.e_)) E(std::move(o.u_.e_));
        }

        ~expected() { destroy(); }

        expected& operator=(expected other) noexcept(
            std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_constructible_v<E>) {
            swap(other);
            return *this;
        }

        // ---------------- observers ----------------
        [[nodiscard]] bool has_value() const noexcept { return has_; }
        [[nodiscard]] explicit operator bool() const noexcept { return has_; }

        [[nodiscard]] T& value()& { assert(has_); return u_.v_; }
        [[nodiscard]] const T& value() const& { assert(has_); return u_.v_; }
        [[nodiscard]] T        value()&& { assert(has_); return std::move(u_.v_); }

        [[nodiscard]] const E& error() const& { assert(!has_); return u_.e_; }
        [[nodiscard]] E& error()& { assert(!has_); return u_.e_; }
        [[nodiscard]] E        error()&& { assert(!has_); return std::move(u_.e_); }

        [[nodiscard]] T& operator*()& { return value(); }
        [[nodiscard]] const T& operator*() const& { return value(); }
        [[nodiscard]] T* operator->() { assert(has_); return std::addressof(u_.v_); }
        [[nodiscard]] const T* operator->() const { assert(has_); return std::addressof(u_.v_); }

        template<class U>
        [[nodiscard]] T value_or(U&& alt) const&
            noexcept(std::is_nothrow_copy_constructible_v<T>&& std::is_nothrow_convertible_v<U, T>) {
            return has_ ? u_.v_ : static_cast<T>(std::forward<U>(alt));
        }
        template<class U>
        [[nodiscard]] T value_or(U&& alt) &&
            noexcept(std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_convertible_v<U, T>) {
            return has_ ? std::move(u_.v_) : static_cast<T>(std::forward<U>(alt));
        }

        // ---------------- modifiers ----------------
        template<class... Args>
        T& emplace(Args&&... args) {
            if (has_) { u_.v_.~T(); }
            else { u_.e_.~E(); }
            ::new (std::addressof(u_.v_)) T(std::forward<Args>(args)...);
            has_ = true;
            return u_.v_;
        }

        template<class... Args>
        E& emplace_error(Args&&... args) {
            if (has_) { u_.v_.~T(); }
            else { u_.e_.~E(); }
            ::new (std::addressof(u_.e_)) E(std::forward<Args>(args)...);
            has_ = false;
            return u_.e_;
        }

        void swap(expected& o) noexcept(
            std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_constructible_v<E>) {
            if (this == &o) return;
            if (has_ && o.has_) {
                using std::swap; swap(u_.v_, o.u_.v_);
            }
            else if (!has_ && !o.has_) {
                using std::swap; swap(u_.e_, o.u_.e_);
            }
            else if (has_ && !o.has_) {
                // this: value, other: error
                E tmp(std::move(o.u_.e_));
                o.u_.e_.~E();
                ::new (std::addressof(o.u_.v_)) T(std::move(u_.v_));
                u_.v_.~T();
                ::new (std::addressof(u_.e_)) E(std::move(tmp));
                std::swap(has_, o.has_);
            }
            else { // !has_ && o.has_
                o.swap(*this);
            }
        }

        // ---------------- tiny monadic helpers ----------------
        template<class F>
        [[nodiscard]] auto and_then(F&& f) &
            -> decltype(std::declval<F&&>()(std::declval<T&>()))
        {
            using Ret = decltype(std::declval<F&&>()(std::declval<T&>()));
            if (has_) return std::forward<F>(f)(u_.v_);
            return Ret(unexpected<E>(u_.e_));
        }

        template<class F>
        [[nodiscard]] auto transform(F&& f) const& {
            using U = std::decay_t<decltype(std::declval<F&>()(std::declval<const T&>()))>;
            if (has_) return expected<U, E>(in_place, f(u_.v_));
            return unexpected<E>(u_.e_);
        }

        template<class F>
        [[nodiscard]] expected<T, E> or_else(F&& f) const& {
            if (has_) return *this;
            f(u_.e_);
            return *this;
        }

    private:
        void destroy() noexcept {
            if (has_) u_.v_.~T();
            else      u_.e_.~E();
        }

        bool has_{ false };
        union U { U() {} ~U() {} T v_; E e_; } u_;
    };

    // ---------------- expected<void,E> (specialization) ----------------

    /**
     * @brief `expected` specialization for `void` values.
     * @tparam E error type (must not be void).
     */
    template<class E>
    class expected<void, E> {
        static_assert(!std::is_void_v<E>, "expected<void, void> is not supported.");
    public:
        using value_type = void;
        using error_type = E;

        // success
        expected() noexcept : has_(true) {}

        // error via unexpected (non-explicit so `return unexpected<E>{...};` works)
        expected(unexpected<E> ue) : has_(false) { ::new (addr()) E(std::move(ue.error())); }

        // error via unexpect tag
        template<class... Args>
        explicit expected(unexpect_t, Args&&... args) : has_(false) {
            ::new (addr()) E(std::forward<Args>(args)...);
        }

        // copy/move
        expected(const expected& o) : has_(o.has_) {
            if (!has_) ::new (addr()) E(*o.err());
        }
        expected(expected&& o) noexcept(std::is_nothrow_move_constructible_v<E>) : has_(o.has_) {
            if (!has_) ::new (addr()) E(std::move(*o.err()));
        }

        ~expected() { destroy(); }

        expected& operator=(expected other) noexcept(std::is_nothrow_move_constructible_v<E>) {
            swap(other); return *this;
        }

        // observers
        [[nodiscard]] bool has_value() const noexcept { return has_; }
        [[nodiscard]] explicit operator bool() const noexcept { return has_; }

        // Like std::expected<void,E>::value(): no value, just check precondition.
        void value() const { assert(has_); }

        [[nodiscard]] const E& error() const& { assert(!has_); return *err(); }
        [[nodiscard]] E& error()& { assert(!has_); return *err(); }
        [[nodiscard]] E        error()&& { assert(!has_); return std::move(*err()); }

        // modifiers
        void emplace() { if (!has_) destroy(); has_ = true; }

        template<class... Args>
        E& emplace_error(Args&&... args) {
            if (!has_) { destroy(); }
            ::new (addr()) E(std::forward<Args>(args)...);
            has_ = false;
            return *err();
        }

        void swap(expected& o) noexcept(std::is_nothrow_move_constructible_v<E>) {
            if (this == &o) return;
            if (has_ && o.has_) { /* nothing to swap */ }
            else if (!has_ && !o.has_) { using std::swap; swap(*err(), *o.err()); }
            else if (has_ && !o.has_) {
                ::new (addr()) E(std::move(*o.err()));
                o.destroy();
                has_ = false; o.has_ = true;
            }
            else { // !has_ && o.has_
                o.swap(*this);
            }
        }

        // convenience
        template<class F>
        [[nodiscard]] auto and_then(F&& f) -> decltype(std::forward<F>(f)()) {
            using Ret = decltype(std::forward<F>(f)());
            if (has_) return std::forward<F>(f)();
            if constexpr (std::is_same_v<Ret, expected<void, E>>) return unexpected<E>(*err());
            else return Ret(unexpected<E>(*err()));
        }
        template<class F>
        [[nodiscard]] expected<void, E> or_else(F&& f) const {
            if (!has_) { f(*err()); }
            return *this;
        }

    private:
        // aligned storage for error
        alignas(E) unsigned char storage_[sizeof(E)];
        bool has_;

        E* err() { return std::launder(reinterpret_cast<E*>(storage_)); }
        const E* err() const { return std::launder(reinterpret_cast<const E*>(storage_)); }
        void* addr() { return static_cast<void*>(storage_); }
        void     destroy() { if (!has_) err()->~E(); }
    };

    // ---------------- free swap (ADL) ----------------
    template<class T, class E>
    void swap(expected<T, E>& a, expected<T, E>& b) noexcept(noexcept(a.swap(b))) { a.swap(b); }

    template<class E>
    void swap(unexpected<E>& a, unexpected<E>& b) noexcept(noexcept(a.swap(b))) { a.swap(b); }

} // namespace Interstellar::IO
#endif
