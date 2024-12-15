// Literals.hpp
#pragma once

#include "Dimensions.h"
namespace Units {
    // Dimensionless Literal
    
    constexpr Scalar operator"" __(long double value) {
        return Scalar(static_cast<float>(value));
    }

    // Length Literals
    constexpr Meter operator"" _m(long double value) {
        return Meter(static_cast<float>(value));
    }

    constexpr Meter operator"" _km(long double value) {
        return Meter(static_cast<float>(value) * 1000.0f);
    }

    constexpr Meter operator"" _cm(long double value) {
        return Meter(static_cast<float>(value) * 0.01f);
    }

    // Area Literals
    constexpr MeterSquared operator"" _m2(long double value) {
        return MeterSquared(static_cast<float>(value));
    }

    // Volume Literals
    constexpr MeterCubed operator"" _m3(long double value) {
        return MeterCubed(static_cast<float>(value));
    }

    // Mass Literals
    constexpr Kilogram operator"" _kg(long double value) {
        return Kilogram(static_cast<float>(value));
    }

    constexpr Kilogram operator"" _g(long double value) {
        return Kilogram(static_cast<float>(value) * 0.001f);
    }

    // Time Literals
    constexpr Second operator"" _s(long double value) {
        return Second(static_cast<float>(value));
    }

    constexpr Second operator"" _ms(long double value) {
        return Second(static_cast<float>(value) * 0.001f);
    }

    // Angle Literals
    constexpr Radian operator"" _rad(long double value) {
        return Radian(static_cast<float>(value));
    }

    // Angular Velocity Literals
    constexpr RadianPerSecond operator"" _rad_s(long double value) {
        return RadianPerSecond(static_cast<float>(value));
    }

    // Angular Acceleration Literals
    constexpr RadianPerSecondSquared operator"" _rad_s2(long double value) {
        return RadianPerSecondSquared(static_cast<float>(value));
    }

    // Force Literals
    constexpr Newton operator"" _N(long double value) {
        return Newton(static_cast<float>(value));
    }

    // Moment of Inertia Literals
    constexpr KilogramMeterSquared operator"" _kg_m2(long double value) {
        return KilogramMeterSquared(static_cast<float>(value));
    }

    // Density Literals
    constexpr KilogramPerCubicMeter operator"" _kg_m3(long double value) {
        return KilogramPerCubicMeter(static_cast<float>(value));
    }

    // Velocity Literals
    constexpr MeterPerSecond operator"" _m_s(long double value) {
        return MeterPerSecond(static_cast<float>(value));
    }

    // Acceleration Literals
    constexpr MeterPerSecondSquared operator"" _m_s2(long double value) {
        return MeterPerSecondSquared(static_cast<float>(value));
    }
} // namespace Units
