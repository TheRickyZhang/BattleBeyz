////////////////////////////////////////////////////////////////////////////////
// Physics.h -- Physics include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Units.h"
using namespace Units;

class BeybladeBody;
class Stadium;

class Physics {
public:
    static constexpr Scalar PI = 3.14159265358979__;

    // Constructor with default parameters
    Physics(float gravity = 9.81f,
        const glm::vec3& gravityVector = glm::vec3(0.0f, -9.81f, 0.0f),
        float frictionEfficiency = 0.5f,
        float frictionAccelConstant = 0.2f,
        float frictionVelocityConstant = 1.5f,
        float fluidDrag = 0.8f)
        : GRAVITY(M_S2(gravity)),
        GRAVITY_VECTOR(Vec3_M_S2(gravityVector)),
        FRICTIONAL_EFFICIENCY(Scalar(frictionEfficiency)),
        FRICTIONAL_ACCELERATION_CONSTANT(Scalar(frictionAccelConstant)),
        FRICTIONAL_VELOCITY_CONSTANT(Scalar(frictionVelocityConstant)),
        FLUID_DRAG(Kg_M3(fluidDrag)) {
    }

    void accumulateAirResistance(BeybladeBody* beyblade) const;
    void accumulateFriction(BeybladeBody* beyblade, Stadium* stadium) const;
    void accumulateSlope(BeybladeBody* beyblade, Stadium* stadium) const;

    // Important: These are not const, as they immediately change position due to contact
    void accumulateImpact(BeybladeBody* beyblade1, BeybladeBody* beyblade2, M contactDistance);
    void preventStadiumClipping(BeybladeBody* beybladeBody, Stadium* stadium);


    M_S2 GRAVITY = 9.81_m_s2;
    Vec3_M_S2 GRAVITY_VECTOR = Vec3_M_S2(0.0f, -9.81f, 0.0f);

    Scalar FRICTIONAL_EFFICIENCY;               // How much loss in angular speed is converted to linear speed due to friction

    // Usually ranges from 0.1 to 0.5.
    Scalar FRICTIONAL_ACCELERATION_CONSTANT;    // Impact of accelaration on frictional force.

    // Usually ranges from 0.01 to 0.1
    Scalar FRICTIONAL_VELOCITY_CONSTANT;        // Impact of angular speed on frictional force with the stadium.

    Kg_M3 FLUID_DRAG;                           // (TODO: replace with airDensity)
};


