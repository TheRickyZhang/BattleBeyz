#pragma once

#include <glm/glm.hpp>
#include "Units.h"

namespace Units {

template<typename Unit>
class Vector3Quantity {
public:
    glm::vec3 value;

    Vector3Quantity() : value(0.0f) {}
    constexpr Vector3Quantity(const glm::vec3& v) : value(v) {}
    constexpr Vector3Quantity(float x, float y, float z) : value(x, y, z) {}

    float x() const { return value.x; }
    float y() const { return value.y; }
    float z() const { return value.z; }

    // Vector addition/subtraction
    constexpr Vector3Quantity<Unit> operator+(const Vector3Quantity<Unit>& rhs) const {
        return Vector3Quantity<Unit>(value + rhs.value);
    }
    constexpr Vector3Quantity<Unit> operator-(const Vector3Quantity<Unit>& rhs) const {
        return Vector3Quantity<Unit>(value - rhs.value);
    }

    // Scalar multiplication and division
    friend constexpr Vector3Quantity<Unit> operator*(float scalar, const Vector3Quantity<Unit>& vec) {
        return Vector3Quantity<Unit>(scalar * vec.value);
    }
    friend constexpr Vector3Quantity<Unit> operator*(const Vector3Quantity<Unit>& vec, float scalar) {
        return Vector3Quantity<Unit>(vec.value * scalar);
    }
    friend constexpr Vector3Quantity<Unit> operator/(const Vector3Quantity<Unit>& vec, float scalar) {
        return Vector3Quantity<Unit>(vec.value / scalar);
    }

    // Compound assignment operators
    Vector3Quantity<Unit>& operator+=(const Vector3Quantity<Unit>& rhs) {
        value += rhs.value;
        return *this;
    }
    Vector3Quantity<Unit>& operator-=(const Vector3Quantity<Unit>& rhs) {
        value -= rhs.value;
        return *this;
    }
    Vector3Quantity<Unit>& operator*=(float scalar) {
        value *= scalar;
        return *this;
    }
    Vector3Quantity<Unit>& operator/=(float scalar) {
        value /= scalar;
        return *this;
    }
};

// TODO: Dot and Cross product should be limited to specific pairs of physically meaningful operations
// with carefulness to ensure corrent functionality

// // Dot product: Vector3Quantity<U> x Vector3Quantity<U> = Scalar<U^2>
// template<typename Unit>
// constexpr Quantity<decltype(Unit{} * Unit{})> dot(
//     const Vector3Quantity<Unit>& lhs, 
//     const Vector3Quantity<Unit>& rhs)
// {
//     float result = glm::dot(lhs.value, rhs.value);
//     using DotUnit = decltype(Unit{} * Unit{});
//     return Quantity<DotUnit>(result);
// }

// // Cross product: Vector3Quantity<U> x Vector3Quantity<U> = Vector3Quantity<U^2>
// template<typename Unit>
// constexpr Vector3Quantity<decltype(Unit{} * Unit{})> cross(
//     const Vector3Quantity<Unit>& lhs,
//     const Vector3Quantity<Unit>& rhs)
// {
//     glm::vec3 result = glm::cross(lhs.value, rhs.value);
//     using CrossUnit = decltype(Unit{} * Unit{});
//     return Vector3Quantity<CrossUnit>(result);
// }

// Additional GLM-style operations
template<typename Unit>
Quantity<Unit> length(const Vector3Quantity<Unit>& vec) {
    return Quantity<Unit>(glm::length(vec.value));
}

template<typename Unit>
Vector3Quantity<Unit> normalize(const Vector3Quantity<Unit>& vec) {
    float len = glm::length(vec.value);
    return (len > 0.0f)
        ? Vector3Quantity<Unit>(vec.value / len)
        : Vector3Quantity<Unit>(0.f, 0.f, 0.f);
}

} // namespace Units
