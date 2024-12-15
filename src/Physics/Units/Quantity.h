// Quantity.hpp
#pragma once

#include "Units.h"
#include <iostream>
#include <type_traits>
#include <cmath>

namespace Units {

    /**
     * @brief Class representing a physical quantity with associated dimensions.
     *
     * @tparam D Dimension type.
     * @tparam T Underlying numerical type (default: float).
     */
    template<typename D, typename T = float>
    class Quantity {
    public:
        // Constructors
        constexpr explicit Quantity(T value) : value_(value) {}

        // Accessor
        constexpr T value() const { return value_; }

        // Unary operators
        constexpr Quantity operator+() const { return *this; }
        constexpr Quantity operator-() const { return Quantity(-value_); }

        // Compound assignment operators
        constexpr Quantity& operator+=(const Quantity& other) {
            value_ += other.value_;
            return *this;
        }

        constexpr Quantity& operator-=(const Quantity& other) {
            value_ -= other.value_;
            return *this;
        }

        constexpr Quantity& operator*=(T scalar) {
            value_ *= scalar;
            return *this;
        }

        constexpr Quantity& operator/=(T scalar) {
            value_ /= scalar;
            return *this;
        }

    private:
        T value_;
    };

    // Addition
    template<typename D, typename T>
    constexpr Quantity<D, T> operator+(const Quantity<D, T>& a, const Quantity<D, T>& b) {
        static_assert(D::template is_same<D>::value, "Cannot add quantities with different dimensions!");
        return Quantity<D, T>(a.value() + b.value());
    }

    // Subtraction
    template<typename D, typename T>
    constexpr Quantity<D, T> operator-(const Quantity<D, T>& a, const Quantity<D, T>& b) {
        static_assert(D::template is_same<D>::value, "Cannot subtract quantities with different dimensions!");
        return Quantity<D, T>(a.value() - b.value());
    }

    // Multiplication (Quantity * Quantity)
    template<typename D1, typename D2, typename T>
    constexpr Quantity<
        Dimension<
        D1::length + D2::length,
        D1::mass + D2::mass,
        D1::time + D2::time,
        D1::angle + D2::angle
        >,
        T
    >
        operator*(const Quantity<D1, T>& a, const Quantity<D2, T>& b) {
        using NewDim = Dimension<
            D1::length + D2::length,
            D1::mass + D2::mass,
            D1::time + D2::time,
            D1::angle + D2::angle
        >;
        return Quantity<NewDim, T>(a.value() * b.value());
    }

    // Division (Quantity / Quantity)
    template<typename D1, typename D2, typename T>
    constexpr Quantity<
        Dimension<
        D1::length - D2::length,
        D1::mass - D2::mass,
        D1::time - D2::time,
        D1::angle - D2::angle
        >,
        T
    >
        operator/(const Quantity<D1, T>& a, const Quantity<D2, T>& b) {
        using NewDim = Dimension<
            D1::length - D2::length,
            D1::mass - D2::mass,
            D1::time - D2::time,
            D1::angle - D2::angle
        >;
        return Quantity<NewDim, T>(a.value() / b.value());
    }

    // Multiplication with scalar (Quantity * scalar)
    template<typename D, typename T>
    constexpr Quantity<D, T> operator*(const Quantity<D, T>& q, T scalar) {
        return Quantity<D, T>(q.value() * scalar);
    }

    // Multiplication with scalar (scalar * Quantity)
    template<typename D, typename T>
    constexpr Quantity<D, T> operator*(T scalar, const Quantity<D, T>& q) {
        return Quantity<D, T>(q.value() * scalar);
    }

    // Division by scalar (Quantity / scalar)
    template<typename D, typename T>
    constexpr Quantity<D, T> operator/(const Quantity<D, T>& q, T scalar) {
        return Quantity<D, T>(q.value() / scalar);
    }

    // Stream insertion for easy printing
    template<typename D, typename T>
    std::ostream& operator<<(std::ostream& os, const Quantity<D, T>& q) {
        os << q.value();
        return os;
    }

} // namespace Units
