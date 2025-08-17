//#pragma once
//#include <type_traits>
//#include "Interstellar/Units/Units.hpp"   // brings in mp-units + sym
//#include "Interstellar/Units/Constants.hpp"
//
//namespace Interstellar::Units::Quantity {
//
//    // -----------------------------
//    // Tags to select construction
//    // -----------------------------
//    struct absolute_t {};
//    struct delta_t {};
//    inline constexpr absolute_t absolute{};
//    inline constexpr delta_t    delta{};
//
//    // -----------------------------
//    // Unit constructors (factories)
//    // -----------------------------
//    // Rule: non-offset units -> construct via operator* (no warnings).
//    //       offset units (temperatures) -> use mp_units::absolute<> / delta<>.
//
//    // Kelvin (offset unit) - safe constructors
//    inline auto K(double v, delta_t) { return ::mp_units::delta<sym::K>(v); }
//    inline auto K(double v, absolute_t) { return ::mp_units::absolute<sym::K>(v); }
//
//    // Common SI (extend as needed)
//    inline auto Pa(double v) { return v * sym::Pa; }
//    inline auto m(double v) { return v * sym::m; }
//    inline auto s(double v) { return v * sym::s; }
//    inline auto kg(double v) { return v * sym::kg; }
//    inline auto mol(double v) { return v * sym::mol; }
//    inline auto m2(double v) { return v * sym::m2; }
//    inline auto m3(double v) { return v * sym::m3; }
//
//    // -----------------------------
//    // User-defined literals (UDLs)
//    // -----------------------------
//    // Provide both floating and integer overloads.
//
//    // Kelvin difference and absolute
//    inline auto operator"" _Kd(long double v) { return ::mp_units::delta<sym::K>(static_cast<double>(v)); }
//    inline auto operator"" _Kd(unsigned long long v) { return ::mp_units::delta<sym::K>(static_cast<double>(v)); }
//    inline auto operator"" _Ka(long double v) { return ::mp_units::absolute<sym::K>(static_cast<double>(v)); }
//    inline auto operator"" _Ka(unsigned long long v) { return ::mp_units::absolute<sym::K>(static_cast<double>(v)); }
//
//    // Non-offset SI examples (add more as needed)
//    inline auto operator"" _Pa(long double v) { return static_cast<double>(v) * sym::Pa; }
//    inline auto operator"" _Pa(unsigned long long v) { return static_cast<double>(v) * sym::Pa; }
//
//    inline auto operator"" _m(long double v) { return static_cast<double>(v) * sym::m; }
//    inline auto operator"" _m(unsigned long long v) { return static_cast<double>(v) * sym::m; }
//
//    inline auto operator"" _s(long double v) { return static_cast<double>(v) * sym::s; }
//    inline auto operator"" _s(unsigned long long v) { return static_cast<double>(v) * sym::s; }
//
//    inline auto operator"" _kg(long double v) { return static_cast<double>(v) * sym::kg; }
//    inline auto operator"" _kg(unsigned long long v) { return static_cast<double>(v) * sym::kg; }
//
//    inline auto operator"" _mol(long double v) { return static_cast<double>(v) * sym::mol; }
//    inline auto operator"" _mol(unsigned long long v) { return static_cast<double>(v) * sym::mol; }
//
//    inline auto operator"" _m2(long double v) { return static_cast<double>(v) * sym::m2; }
//    inline auto operator"" _m2(unsigned long long v) { return static_cast<double>(v) * sym::m2; }
//
//    inline auto operator"" _m3(long double v) { return static_cast<double>(v) * sym::m3; }
//    inline auto operator"" _m3(unsigned long long v) { return static_cast<double>(v) * sym::m3; }
//
//    // -----------------------------
//    // Normalization helpers
//    // -----------------------------
//    // Convert any temperature-like (absolute point or delta) to delta-K
//    template<class T>
//    constexpr auto as_deltaK(const T& t) {
//        if constexpr (::mp_units::QuantityPoint<T>) {
//            return t - ::mp_units::si::absolute_zero; // absolute K -> delta K
//        }
//        else {
//            return t; // already delta K
//        }
//    }
//
//    // -----------------------------
//    // Scalar extraction helpers
//    // -----------------------------
//    // Generic: get raw number in the requested unit
//    template<class Q, class UnitSymbol>
//    inline double number_in(const Q& q, UnitSymbol u) {
//        return static_cast<double>(q / (1.0 * u));
//    }
//
//    // Convenience wrappers
//    template<class Q> inline double in_Pa(const Q& q) { return number_in(q, sym::Pa); }
//    template<class Q> inline double in_K(const Q& q) { return number_in(q, sym::K); } // works for delta-K
//
//    // For absolute-K points, return the numeric kelvin
//    template<class QP>
//    inline double absK_number(const QP& q_abs) {
//        static_assert(::mp_units::QuantityPoint<QP>);
//        // absolute point - absolute_zero -> delta-K, then divide by K
//        return number_in(q_abs - ::mp_units::si::absolute_zero, sym::K);
//    }
//
//} // namespace Interstellar::Units::qty
