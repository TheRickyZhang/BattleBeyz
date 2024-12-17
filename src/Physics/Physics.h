////////////////////////////////////////////////////////////////////////////////
// Physics.h -- Physics include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include "Utils.h"
#include "RigidBodies/BeybladeBody.h"
#include "RigidBodies/StadiumBody.h"

#include "Units.h"
using namespace Units;

namespace Physics {
    constexpr M_S2 GRAVITY = -9.81_m_s2;
    constexpr Vec3_M_S2 GRAVITY_VECTOR = Vec3_M_S2(0.0f, -9.81f, 0.0f);

    // How much loss in angular speed is converted to linear speed due to friction
    constexpr Scalar FRICTIONAL_EFFICIENCY = 0.5__;

    // How much impact the accelaration has on frictional force. Usually ranges from 0.1 to 0.5.
    // Follows a = g * mu * cos * direction vector (* FAC)
    constexpr Scalar FRICTIONAL_ACCELERATION_CONSTANT = 0.17__;

    // How much impact we want the angular speed affects the frictional force with the stadium. Usually ranges from 0.01 to 0.1
    // Follows a = w * mu (* FVC)
    constexpr Scalar FRICTIONAL_VELOCITY_CONSTANT = 0.04__;

    // How much aerial drag there is
    const Kg_M3 FLUID_DRAG = 0.08_kg / (1.0_m * 1.0_m * 1.0_m);
    void accumulateAirResistance(BeybladeBody* beyblade, Kg_M3 airDensity = FLUID_DRAG);

    void accumulateFriction(BeybladeBody* beyblade, StadiumBody* stadium);
    void accumulateSlope(BeybladeBody* beyblade, StadiumBody* stadium);
    void accumulateImpact(BeybladeBody* beyblade1, BeybladeBody* beyblade2, M contactDistance);
    void preventStadiumClipping(BeybladeBody* beybladeBody, StadiumBody* stadiumBody);
}


