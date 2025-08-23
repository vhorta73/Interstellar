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

/// \file
/// \ingroup IO
/// \brief Aliases to C++23 std::expected where available.
/// \details
///  - When std::expected is available, this header aliases the standard types.
///  - When not, a small fallback with a subset of the standard interface is provided.
///  - Exception policy for fallback: value()/error() assert on misuse (no throws).

namespace Interstellar::IO {
    /// \brief Alias to std::expected when available.
    template<class T, class E> using expected = std::expected<T, E>;
    /// \brief Alias to std::unexpected when available.
    template<class E>         using unexpected = std::unexpected<E>;
    /// \brief Alias to std::make_unexpected when available.
    using std::make_unexpected;
    /// \brief Alias to std::unexpect_t when available.
    using std::unexpect_t;
    /// \brief Tag object to construct error alternative.
    inline constexpr std::unexpect_t unexpect{};
} // namespace Interstellar::IO

#else // --------------------------- fallback shim ---------------------------

#include <new>          // placement new, std::launder
#include <utility>      // move, forward, addressof
#include <type_traits>  // traits
#include <cassert>      // assert
#include <cstddef>      // size_t

/// \file
/// \ingroup IO
/// \brief Minimal std::expected-like types for environments without C++23.
/// \details
///  - Recovers a practical subset of std::expected.
///  - No exceptions are thrown by this shim; misuse is caught via assert.
///  - Monadic helpers (and_then, transform, or_else) are provided in a lightweight form.
///  - Designed for internal use within Interstellar IO.

namespace Interstellar::IO {

    // ---------------- tag types ----------------

    /**
     * @ingroup IO
     * @brief Tag type signaling construction of the error alternative.
     * @since 1.0
     */
    struct unexpect_t { explicit unexpect_t() = default; };

    /// @ingroup IO
    /// @brief Tag object for constructing the error alternative.
    inline constexpr unexpect_t unexpect{};

    /**
     * @ingroup IO
     * @brief Tag type signaling in-place value construction.
     * @since 1.0
     */
    struct in_place_t { explicit in_place_t() = default; };

    /// @ingroup IO
    /// @brief Tag object for in-place value construction.
    inline constexpr in_place_t in_place{};

    // ---------------- unexpected<E> ----------------

    /**
     * @ingroup IO
     * @brief Holder for an error value used to construct an expected in its error state.
     * @tparam E Error type (must not be void).
     * @note Matches the intent of std::unexpected.
     * @since 1.0
     */
    template<class E>
    class unexpected {
        static_assert(!std::is_void_v<E>, "unexpected<void> is not supported.");
    public:
        using error_type = E;

        /**
         * @brief Construct from an lvalue error (explicit to avoid accidental conversions).
         * @param e Error instance.
         * @since 1.0
         */
        explicit unexpected(const E& e) : e_(e) {}

        /**
         * @brief Construct from an rvalue error.
         * @param e Error instance (moved).
         * @since 1.0
         */
        explicit unexpected(E&& e) noexcept(std::is_nothrow_move_constructible_v<E>) : e_(std::move(e)) {}

        /// @brief Access the stored error (const lvalue).
        [[nodiscard]] const E& error() const& noexcept { return e_; }
        /// @brief Access the stored error (lvalue).
        [[nodiscard]] E& error() & noexcept { return e_; }
        /// @brief Move out the stored error (rvalue).
        [[nodiscard]] E        error() && noexcept(std::is_nothrow_move_constructible_v<E>) { return std::move(e_); }

        /// @brief Swap errors.
        void swap(unexpected& other) noexcept(std::is_nothrow_swappable_v<E>) { using std::swap; swap(e_, other.e_); }

        /// @brief Equality compares the contained error values.
        friend bool operator==(const unexpected& a, const unexpected& b) { return a.e_ == b.e_; }
        /// @brief Inequality compares the contained error values.
        friend bool operator!=(const unexpected& a, const unexpected& b) { return !(a == b); }

    private:
        E e_;
    };

    /**
     * @ingroup IO
     * @brief Factory to create unexpected<E> without colliding with global ::unexpected.
     * @tparam Err Error-like type.
     * @param e Error to wrap.
     * @return unexpected<decayed Err>
     * @since 1.0
     */
    template<class Err>
    [[nodiscard]] unexpected<std::decay_t<Err>> make_unexpected(Err&& e) {
        return unexpected<std::decay_t<Err>>(std::forward<Err>(e));
    }

    // ---------------- expected<T,E> (primary) ----------------

    /**
     * @ingroup IO
     * @brief Discriminated union of either a value T or an error E.
     * @tparam T Value type (not void in this primary template).
     * @tparam E Error type (must not be void).
     * @details
     * - No exceptions thrown by this type; value()/error() assert on the wrong alternative.
     * - Provides a focused subset of the C++23 std::expected interface.
     * @since 1.0
     */
    template<class T, class E>
    class expected {
        static_assert(!std::is_void_v<T>, "expected<void, E> has a specialization.");
        static_assert(!std::is_void_v<E>, "expected<T, void> is not supported.");
    public:
        using value_type = T;
        using error_type = E;

        // ---------------- constructors: success state ----------------

        /**
         * @brief Default-construct the value.
         * @requires T is default-constructible.
         * @since 1.0
         */
        expected() noexcept(std::is_nothrow_default_constructible_v<T>)
            requires std::is_default_constructible_v<T>
        : has_(true) {
            ::new (std::addressof(u_.v_)) T();
        }

        /**
         * @brief Construct from an lvalue value.
         * @param v Value to copy.
         * @since 1.0
         */
        expected(const T& v) : has_(true) { ::new (std::addressof(u_.v_)) T(v); }

        /**
         * @brief Construct from an rvalue value.
         * @param v Value to move.
         * @since 1.0
         */
        expected(T&& v) noexcept(std::is_nothrow_move_constructible_v<T>) : has_(true) {
            ::new (std::addressof(u_.v_)) T(std::move(v));
        }

        /**
         * @brief In-place construct the value.
         * @tparam Args Constructor arg types.
         * @param args Forwarded args.
         * @since 1.0
         */
        template<class... Args>
        explicit expected(in_place_t, Args&&... args)
            : has_(true) {
            ::new (std::addressof(u_.v_)) T(std::forward<Args>(args)...);
        }

        // ---------------- constructors: error state ----------------

        /**
         * @brief Construct the error alternative from unexpected<E>.
         * @param ue Error holder; moved-from.
         * @note Intentionally non-explicit so `return unexpected<E>{...};` works.
         * @since 1.0
         */
        expected(unexpected<E> ue) : has_(false) { ::new (std::addressof(u_.e_)) E(std::move(ue.error())); }

        /**
         * @brief In-place construct the error alternative.
         * @tparam Args Error constructor arg types.
         * @param args Forwarded args.
         * @since 1.0
         */
        template<class... Args>
        explicit expected(unexpect_t, Args&&... args) : has_(false) {
            ::new (std::addressof(u_.e_)) E(std::forward<Args>(args)...);
        }

        // ---------------- copy/move ----------------

        /// @brief Copy-construct from another expected.
        expected(const expected& o) : has_(o.has_) {
            if (has_) ::new (std::addressof(u_.v_)) T(o.u_.v_);
            else      ::new (std::addressof(u_.e_)) E(o.u_.e_);
        }

        /// @brief Move-construct from another expected.
        expected(expected&& o) noexcept(
            std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_constructible_v<E>)
            : has_(o.has_) {
            if (has_) ::new (std::addressof(u_.v_)) T(std::move(o.u_.v_));
            else      ::new (std::addressof(u_.e_)) E(std::move(o.u_.e_));
        }

        /// @brief Destructor destroys the active alternative.
        ~expected() { destroy(); }

        /// @brief Copy-assign via copy-and-swap.
        expected& operator=(expected other) noexcept(
            std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_constructible_v<E>) {
            swap(other);
            return *this;
        }

        // ---------------- observers ----------------

        /// @brief True if a value is present.
        [[nodiscard]] bool has_value() const noexcept { return has_; }

        /// @brief Contextual conversion to bool; same as has_value().
        [[nodiscard]] explicit operator bool() const noexcept { return has_; }

        /// @brief Access value (lvalue). Precondition: has_value() is true.
        [[nodiscard]] T& value()& { assert(has_); return u_.v_; }
        /// @brief Access value (const lvalue). Precondition: has_value() is true.
        [[nodiscard]] const T& value() const& { assert(has_); return u_.v_; }
        /// @brief Move value (rvalue). Precondition: has_value() is true.
        [[nodiscard]] T        value()&& { assert(has_); return std::move(u_.v_); }

        /// @brief Access error (const lvalue). Precondition: has_value() is false.
        [[nodiscard]] const E& error() const& { assert(!has_); return u_.e_; }
        /// @brief Access error (lvalue). Precondition: has_value() is false.
        [[nodiscard]] E& error()& { assert(!has_); return u_.e_; }
        /// @brief Move error (rvalue). Precondition: has_value() is false.
        [[nodiscard]] E        error()&& { assert(!has_); return std::move(u_.e_); }

        /// @brief Dereference to the value. Precondition: has_value() is true.
        [[nodiscard]] T& operator*()& { return value(); }
        /// @brief Dereference to the value. Precondition: has_value() is true.
        [[nodiscard]] const T& operator*() const& { return value(); }
        /// @brief Pointer-like access to value. Precondition: has_value() is true.
        [[nodiscard]] T* operator->() { assert(has_); return std::addressof(u_.v_); }
        /// @brief Pointer-like access to value. Precondition: has_value() is true.
        [[nodiscard]] const T* operator->() const { assert(has_); return std::addressof(u_.v_); }

        /**
         * @brief Return value or a fallback if no value.
         * @tparam U Convertible to T.
         * @param alt Fallback value.
         * @return T
         * @since 1.0
         */
        template<class U>
        [[nodiscard]] T value_or(U&& alt) const&
            noexcept(std::is_nothrow_copy_constructible_v<T>&& std::is_nothrow_convertible_v<U, T>) {
            return has_ ? u_.v_ : static_cast<T>(std::forward<U>(alt));
        }
        /// @overload
        template<class U>
        [[nodiscard]] T value_or(U&& alt) &&
            noexcept(std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_convertible_v<U, T>) {
            return has_ ? std::move(u_.v_) : static_cast<T>(std::forward<U>(alt));
        }

        // ---------------- modifiers ----------------

        /**
         * @brief Emplace-construct a new value, replacing any error.
         * @tparam Args Constructor arg types.
         * @param args Forwarded args.
         * @return T& reference to the newly constructed value.
         * @since 1.0
         */
        template<class... Args>
        T& emplace(Args&&... args) {
            if (has_) { u_.v_.~T(); }
            else { u_.e_.~E(); }
            ::new (std::addressof(u_.v_)) T(std::forward<Args>(args)...);
            has_ = true;
            return u_.v_;
        }

        /**
         * @brief Emplace-construct a new error, replacing any value.
         * @tparam Args Error constructor arg types.
         * @param args Forwarded args.
         * @return E& reference to the newly constructed error.
         * @since 1.0
         */
        template<class... Args>
        E& emplace_error(Args&&... args) {
            if (has_) { u_.v_.~T(); }
            else { u_.e_.~E(); }
            ::new (std::addressof(u_.e_)) E(std::forward<Args>(args)...);
            has_ = false;
            return u_.e_;
        }

        /**
         * @brief Swap contents with another expected.
         * @param o Other expected.
         * @since 1.0
         */
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

        /**
         * @brief If value is present, invoke f(T&) and return its result; otherwise propagate error.
         * @tparam F Callable taking T& and returning expected<U,E> (or compatible).
         * @param f Function to invoke.
         * @return expected<U,E>-like result from f, or error.
         * @since 1.0
         */
        template<class F>
        [[nodiscard]] auto and_then(F&& f) &
            -> decltype(std::declval<F&&>()(std::declval<T&>()))
        {
            using Ret = decltype(std::declval<F&&>()(std::declval<T&>()));
            if (has_) return std::forward<F>(f)(u_.v_);
            return Ret(unexpected<E>(u_.e_));
        }

        /**
         * @brief Map the contained value through f if present; otherwise propagate error.
         * @tparam F Callable taking const T& and returning U.
         * @param f Function to apply.
         * @return expected<U,E> with transformed value, or error.
         * @since 1.0
         */
        template<class F>
        [[nodiscard]] auto transform(F&& f) const& {
            using U = std::decay_t<decltype(std::declval<F&>()(std::declval<const T&>()))>;
            if (has_) return expected<U, E>(in_place, f(u_.v_));
            return unexpected<E>(u_.e_);
        }

        /**
         * @brief If in error, call f(E) for side effects and return this.
         * @tparam F Callable taking const E& or E&.
         * @param f Function to invoke on error.
         * @return expected<T,E> unchanged.
         * @since 1.0
         */
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
     * @ingroup IO
     * @brief expected specialization for void values.
     * @tparam E Error type (must not be void).
     * @since 1.0
     */
    template<class E>
    class expected<void, E> {
        static_assert(!std::is_void_v<E>, "expected<void, void> is not supported.");
    public:
        using value_type = void;
        using error_type = E;

        /// @brief Construct success state.
        expected() noexcept : has_(true) {}

        /**
         * @brief Construct error state from unexpected<E>.
         * @param ue Error holder; moved-from.
         * @note Intentionally non-explicit so `return unexpected<E>{...};` works.
         */
        expected(unexpected<E> ue) : has_(false) { ::new (addr()) E(std::move(ue.error())); }

        /**
         * @brief In-place construct error state.
         * @tparam Args Error constructor arg types.
         * @param args Forwarded args.
         */
        template<class... Args>
        explicit expected(unexpect_t, Args&&... args) : has_(false) {
            ::new (addr()) E(std::forward<Args>(args)...);
        }

        /// @brief Copy-construct.
        expected(const expected& o) : has_(o.has_) {
            if (!has_) ::new (addr()) E(*o.err());
        }
        /// @brief Move-construct.
        expected(expected&& o) noexcept(std::is_nothrow_move_constructible_v<E>) : has_(o.has_) {
            if (!has_) ::new (addr()) E(std::move(*o.err()));
        }

        /// @brief Destroy active alternative.
        ~expected() { destroy(); }

        /// @brief Copy-assign via copy-and-swap.
        expected& operator=(expected other) noexcept(std::is_nothrow_move_constructible_v<E>) {
            swap(other); return *this;
        }

        // observers

        /// @brief True if success.
        [[nodiscard]] bool has_value() const noexcept { return has_; }
        /// @brief Contextual conversion to bool; same as has_value().
        [[nodiscard]] explicit operator bool() const noexcept { return has_; }

        /**
         * @brief Check success; asserts if in error.
         * @details Like std::expected<void,E>::value(): no value to return.
         */
        void value() const { assert(has_); }

        /// @brief Access error (const lvalue). Precondition: !has_value().
        [[nodiscard]] const E& error() const& { assert(!has_); return *err(); }
        /// @brief Access error (lvalue). Precondition: !has_value().
        [[nodiscard]] E& error()& { assert(!has_); return *err(); }
        /// @brief Move error (rvalue). Precondition: !has_value().
        [[nodiscard]] E        error()&& { assert(!has_); return std::move(*err()); }

        // modifiers

        /// @brief Emplace success, destroying any error.
        void emplace() { if (!has_) destroy(); has_ = true; }

        /**
         * @brief Emplace-construct a new error, replacing success.
         * @tparam Args Error constructor arg types.
         * @param args Forwarded args.
         * @return E& reference to the newly constructed error.
         */
        template<class... Args>
        E& emplace_error(Args&&... args) {
            if (!has_) { destroy(); }
            ::new (addr()) E(std::forward<Args>(args)...);
            has_ = false;
            return *err();
        }

        /**
         * @brief Swap contents with another expected<void,E>.
         * @param o Other expected.
         */
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

        /**
         * @brief If success, call f() and return its result; otherwise propagate error.
         * @tparam F Callable returning expected<*,E>-compatible.
         * @param f Function to invoke.
         * @return Result of f() on success; otherwise unexpected<E>(error()).
         */
        template<class F>
        [[nodiscard]] auto and_then(F&& f) -> decltype(std::forward<F>(f)()) {
            using Ret = decltype(std::forward<F>(f)());
            if (has_) return std::forward<F>(f)();
            if constexpr (std::is_same_v<Ret, expected<void, E>>) return unexpected<E>(*err());
            else return Ret(unexpected<E>(*err()));
        }

        /**
         * @brief If in error, call f(E) for side effects and return this.
         * @tparam F Callable taking const E& or E&.
         * @param f Function to invoke on error.
         * @return expected<void,E> unchanged.
         */
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
        void  destroy() { if (!has_) err()->~E(); }
    };

    // ---------------- free swap (ADL) ----------------

    /// @ingroup IO
    /// @brief Swap two expected<T,E> instances.
    template<class T, class E>
    void swap(expected<T, E>& a, expected<T, E>& b) noexcept(noexcept(a.swap(b))) { a.swap(b); }

    /// @ingroup IO
    /// @brief Swap two unexpected<E> instances.
    template<class E>
    void swap(unexpected<E>& a, unexpected<E>& b) noexcept(noexcept(a.swap(b))) { a.swap(b); }

} // namespace Interstellar::IO
#endif
