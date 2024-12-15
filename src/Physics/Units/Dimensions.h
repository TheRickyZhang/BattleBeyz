// Dimensions.hpp
#pragma once

#include "Units.h"
#include "Quantity.h"

namespace Units {
    // **Base Dimensions**
    using Dimensionless = Dimension<0, 0, 0, 0>; // No dimension (e.g., coefficient of restitution, drag coefficients)
    using LengthDim = Dimension<1, 0, 0, 0>;     // L (Length)
    using MassDim = Dimension<0, 1, 0, 0>;       // M (Mass)
    using TimeDim = Dimension<0, 0, 1, 0>;       // T (Time)
    using AngleDim = Dimension<0, 0, 0, 1>;      // A (Angle, e.g., radians)

    using AreaDim = Dimension<2, 0, 0, 0>;                     // L^2
    using VolumeDim = Dimension<3, 0, 0, 0>;                   // L^3

    // **Quantity Type Aliases**
    using Scalar = Quantity<Dimensionless, float>;               // Coefficients, ratios, etc.
    using Meter = Quantity<LengthDim, float>;                    // Length (meters)
    using Kilogram = Quantity<MassDim, float>;                   // Mass (kilograms)
    using Second = Quantity<TimeDim, float>;                     // Time (seconds)
    using Degree = Quantity<AngleDim, float>;                    // Degrees (angle)
    using Radian = Quantity<AngleDim, float>;                    // Radians (angle)

    // **Derived Dimensions**
    //using VelocityDim = Dimension<1, 0, -1, 0>;                // L/T (Linear Velocity)
    //using AccelerationDim = Dimension<1, 0, -2, 0>;            // L/T² (Linear Acceleration)
    //using AngularVelocityDim = Dimension<0, 0, -1, 1>;         // A/T (Angular Velocity)
    //using AngularAccelerationDim = Dimension<0, 0, -2, 1>;     // A/T² (Angular Acceleration)
    //using ForceDim = Dimension<1, 1, -2, 0>;                   // M*L/T² (Force, useful for impacts)
    //using TorqueDim = Dimension<2, 1, -2, 0>;                  // M*L²/T² (Torque, for rotational forces)
    //using MomentumDim = Dimension<1, 1, -1, 0>;                // M*L/T (Linear Momentum)
    //using AngularMomentumDim = Dimension<2, 1, -1, 0>;         // M*L²/T (Angular Momentum, useful for spin calculations)
    //using InertiaDim = Dimension<2, 1, 0, 0>;                  // M*L² (Moment of Inertia, for spin and rotation physics)
    //using LinearMassDensity = Dimension<-1, 1, 0, 0>;          // M/L  
    //using AirDensityDim = Dimension<-3, 1, 0, 0>;              // M/L³ (Air Density, for drag calculations if needed)

    /* **Derived Quantity Type Aliases**
    * 
    * Syntax: X_Y -> Units X / Units Y
    * 
    * M = Meters (Length)
    * Kg = Kilograms (Mass)
    * S = Seconds (Time)
    * R = Radians (Angle)
    * 
    * When combining multiple variables, use order: R, Kg, M, S
    */

    using R_S = Quantity<Dimension<0, 0, -1, 1>, float>;       // Angular velocity (rad/s)
    using R_S2 = Quantity<Dimension<0, 0, -2, 1>, float>;      // Angular acceleration (rad/s²)

    using KgM_S2 = Quantity<Dimension<1, 1, -2, 0>, float>;    // Force (N = kg·m/s²)
    using KgM2_S2 = Quantity<Dimension<2, 1, -2, 0>, float>;   // Torque (kg·m²/s²)
    using KgM_S = Quantity<Dimension<1, 1, -1, 0>, float>;     // Momentum (kg·m/s)
    using KgM2_S = Quantity<Dimension<2, 1, -1, 0>, float>;    // Angular momentum (kg·m²/s)
    using KgM2 = Quantity<Dimension<2, 1, 0, 0>, float>;       // Inertia (kg·m²)
    using Kg_M = Quantity<Dimension<-1, 1, 0, 0>, float>;      // Linear mass density (kg/m)
    using Kg_M3 = Quantity<Dimension<-3, 1, 0, 0>, float>;     // Density (kg/m³)

    using M2 = Quantity<Dimension<2, 0, 0, 0>, float>;         // Area (m²)
    using M3 = Quantity<Dimension<3, 0, 0, 0>, float>;         // Volume (m³)
    using M4 = Quantity<Dimension<4, 0, 0, 0>, float>;         // Higher power for geometry
    using M_S = Quantity<Dimension<1, 0, -1, 0>, float>;       // Velocity (m/s)
    using M_S2 = Quantity<Dimension<1, 0, -2, 0>, float>;      // Acceleration (m/s²)

}
