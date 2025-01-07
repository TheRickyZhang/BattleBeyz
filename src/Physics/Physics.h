////////////////////////////////////////////////////////////////////////////////
// Physics.h -- Physics include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Units.h"
using namespace Units;

class BeybladeBody;
class StadiumBody;

class Physics {
public:
    const Scalar PI = 3.14159265358979__;

    // Constructor with default parameters
    Physics(M_S2 gravity = 9.81_m_s2,
        Vec3_M_S2 gravityVector = Vec3_M_S2(0.0f, -9.81f, 0.0f),
        Scalar frictionEfficiency = 0.5__,
        Scalar frictionAccelConstant = 0.2__,
        Scalar frictionVelocityConstant = 1.5__,
        Kg_M3 fluidDrag = 0.8_kg / (1.0_m * 1.0_m * 1.0_m))
        : GRAVITY(gravity),
        GRAVITY_VECTOR(gravityVector),
        FRICTIONAL_EFFICIENCY(frictionEfficiency),
        FRICTIONAL_ACCELERATION_CONSTANT(frictionAccelConstant),
        FRICTIONAL_VELOCITY_CONSTANT(frictionVelocityConstant),
        FLUID_DRAG(fluidDrag) {
    }

    void accumulateAirResistance(BeybladeBody* beyblade) const;
    void accumulateFriction(BeybladeBody* beyblade, StadiumBody* stadium) const;
    void accumulateSlope(BeybladeBody* beyblade, StadiumBody* stadium) const;

    // Important: These are not const, as they immediately change position due to contact
    void accumulateImpact(BeybladeBody* beyblade1, BeybladeBody* beyblade2, M contactDistance);
    void preventStadiumClipping(BeybladeBody* beybladeBody, StadiumBody* stadiumBody);


    M_S2 GRAVITY = 9.81_m_s2;
    Vec3_M_S2 GRAVITY_VECTOR = Vec3_M_S2(0.0f, -9.81f, 0.0f);

    Scalar FRICTIONAL_EFFICIENCY;               // How much loss in angular speed is converted to linear speed due to friction

    // Usually ranges from 0.1 to 0.5.
    Scalar FRICTIONAL_ACCELERATION_CONSTANT;    // Impact of accelaration on frictional force.

    // Usually ranges from 0.01 to 0.1
    Scalar FRICTIONAL_VELOCITY_CONSTANT;        // Impact of angular speed on frictional force with the stadium.

    Kg_M3 FLUID_DRAG;                           // (TODO: replace with airDensity)
};


