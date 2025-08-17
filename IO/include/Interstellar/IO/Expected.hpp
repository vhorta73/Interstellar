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

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
#include <expected>
namespace Interstellar::IO {
    template<class T, class E> using expected = std::expected<T, E>;
    template<class E>         using unexpected = std::unexpected<E>;
    using std::make_unexpected;
    using std::unexpect_t;
    inline constexpr std::unexpect_t unexpect{};
}
#else
#include <new>          // placement new, std::launder
#include <utility>      // std::move, std::forward
#include <type_traits>  // type traits
#include <cassert>      // assert

namespace Interstellar::IO {

    // ---------------- tag type (mirrors std::unexpect_t) ----------------
    struct unexpect_t { explicit unexpect_t() = default; };
    inline constexpr unexpect_t unexpect{};

    // ---------------- unexpected<E> ----------------
    template<class E>
    class unexpected {
        static_assert(!std::is_void_v<E>,
            "unexpected<void> is not supported in this shim; use a real error type.");
    public:
        using error_type = E;

        unexpected(const E& e) : e_(e) {}
        unexpected(E&& e) : e_(std::move(e)) {}

        // Observers
        [[nodiscard]] const E& error() const& { return e_; }
        [[nodiscard]] E& error()& { return e_; }
        [[nodiscard]] E        error()&& { return std::move(e_); }

    private:
        E e_;
    };

    // Factory to avoid MSVC's global ::unexpected() name clash.
    template<class Err>
    [[nodiscard]] unexpected<std::decay_t<Err>> make_unexpected(Err&& e) {
        return unexpected<std::decay_t<Err>>(std::forward<Err>(e));
    }

    // ---------------- expected<T,E> (general) ----------------
    template<class T, class E>
    class expected {
        static_assert(!std::is_void_v<T>,
            "expected<void, E> handled by specialization below.");
        static_assert(!std::is_void_v<E>,
            "expected<T, void> is not supported in this shim.");
    public:
        using value_type = T;
        using error_type = E;

        // success
        expected(const T& v) : has_(true) { ::new (&u_.v_) T(v); }
        expected(T&& v) : has_(true) { ::new (&u_.v_) T(std::move(v)); }

        // error via unexpected (intentionally non-explicit to allow `return unexpected<E>{...};`)
        expected(unexpected<E> ue) : has_(false) { ::new (&u_.e_) E(std::move(ue.error())); }

        // error via unexpect tag
        template<class... Args>
        explicit expected(unexpect_t, Args&&... args) : has_(false) {
            ::new (&u_.e_) E(std::forward<Args>(args)...);
        }

        // copy/move
        expected(const expected& o) : has_(o.has_) {
            if (has_) ::new (&u_.v_) T(o.u_.v_); else ::new (&u_.e_) E(o.u_.e_);
        }
        expected(expected&& o) noexcept(
            std::is_nothrow_move_constructible_v<T>&&
            std::is_nothrow_move_constructible_v<E>) : has_(o.has_) {
            if (has_) ::new (&u_.v_) T(std::move(o.u_.v_));
            else      ::new (&u_.e_) E(std::move(o.u_.e_));
        }

        ~expected() { destroy(); }

        expected& operator=(expected other) noexcept(
            std::is_nothrow_move_constructible_v<T>&&
            std::is_nothrow_move_constructible_v<E>) {
            swap(other); return *this;
        }

        // Observers
        [[nodiscard]] bool has_value() const noexcept { return has_; }
        [[nodiscard]] explicit operator bool() const noexcept { return has_; }

        [[nodiscard]] T& value()& { assert(has_); return u_.v_; }
        [[nodiscard]] const T& value() const& { assert(has_); return u_.v_; }
        [[nodiscard]] T        value()&& { assert(has_); return std::move(u_.v_); }

        [[nodiscard]] const E& error() const& { assert(!has_); return u_.e_; }
        [[nodiscard]] E& error()& { assert(!has_); return u_.e_; }
        [[nodiscard]] E        error()&& { assert(!has_); return std::move(u_.e_); }

        // Modifiers
        template<class... Args>
        T& emplace(Args&&... args) {
            if (has_) { u_.v_.~T(); }
            else { u_.e_.~E(); }
            ::new (&u_.v_) T(std::forward<Args>(args)...);
            has_ = true;
            return u_.v_;
        }

        template<class... Args>
        E& emplace_error(Args&&... args) {
            if (has_) { u_.v_.~T(); }
            else { u_.e_.~E(); }
            ::new (&u_.e_) E(std::forward<Args>(args)...);
            has_ = false;
            return u_.e_;
        }

        void swap(expected& o) noexcept(
            std::is_nothrow_move_constructible_v<T>&&
            std::is_nothrow_move_constructible_v<E>) {
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
                ::new (&o.u_.v_) T(std::move(u_.v_));
                u_.v_.~T();
                ::new (&u_.e_) E(std::move(tmp));
                std::swap(has_, o.has_);
            }
            else { // !has_ && o.has_
                // this: error, other: value
                o.swap(*this); // reuse the branch above
            }
        }

    private:
        void destroy() {
            if (has_) u_.v_.~T();
            else      u_.e_.~E();
        }

        bool has_;
        union U { U() {} ~U() {} T v_; E e_; } u_;
    };

    // ---------------- expected<void,E> (specialization) ----------------
    template<class E>
    class expected<void, E> {
        static_assert(!std::is_void_v<E>,
            "expected<void, void> is not supported in this shim.");
    public:
        using value_type = void;
        using error_type = E;

        // success
        expected() noexcept : has_(true) {}

        // error via unexpected (non-explicit so `return unexpected<E>{...};` works)
        expected(unexpected<E> ue) : has_(false) {
            ::new (addr()) E(std::move(ue.error()));
        }

        // error via unexpect tag
        template<class... Args>
        explicit expected(unexpect_t, Args&&... args) : has_(false) {
            ::new (addr()) E(std::forward<Args>(args)...);
        }

        // copy/move
        expected(const expected& o) : has_(o.has_) {
            if (!has_) ::new (addr()) E(*o.err());
        }
        expected(expected&& o) noexcept(std::is_nothrow_move_constructible_v<E>)
            : has_(o.has_) {
            if (!has_) ::new (addr()) E(std::move(*o.err()));
        }

        ~expected() { destroy(); }

        expected& operator=(expected other) noexcept(
            std::is_nothrow_move_constructible_v<E>) {
            swap(other); return *this;
        }

        // Observers
        [[nodiscard]] bool has_value() const noexcept { return has_; }
        [[nodiscard]] explicit operator bool() const noexcept { return has_; }

        // Like std::expected<void,E>::value(): no value, just check
        void value() const { assert(has_); }

        [[nodiscard]] const E& error() const& { assert(!has_); return *err(); }
        [[nodiscard]] E& error()& { assert(!has_); return *err(); }
        [[nodiscard]] E        error()&& { assert(!has_); return std::move(*err()); }

        // Modifiers
        void emplace() {
            if (!has_) { destroy(); }
            has_ = true;
        }

        template<class... Args>
        E& emplace_error(Args&&... args) {
            if (!has_) { destroy(); }
            ::new (addr()) E(std::forward<Args>(args)...);
            has_ = false;
            return *err();
        }

        void swap(expected& o) noexcept(std::is_nothrow_move_constructible_v<E>) {
            if (this == &o) return;

            if (has_ && o.has_) {
                // both success: nothing to swap
            }
            else if (!has_ && !o.has_) {
                using std::swap; swap(*err(), *o.err());
            }
            else if (has_ && !o.has_) {
                ::new (addr()) E(std::move(*o.err()));
                o.destroy();
                has_ = false; o.has_ = true;
            }
            else { // !has_ && o.has_
                o.swap(*this);
            }
        }

    private:
        // Use an aligned byte buffer instead of a union (avoids MSVC deleted dtor issue)
        alignas(E) unsigned char storage_[sizeof(E)];
        bool has_;

        E* err() { return std::launder(reinterpret_cast<E*>(storage_)); }
        const E* err() const { return std::launder(reinterpret_cast<const E*>(storage_)); }
        void* addr() { return static_cast<void*>(storage_); }

        void destroy() { if (!has_) err()->~E(); }
    };

} // namespace Interstellar::IO
#endif
