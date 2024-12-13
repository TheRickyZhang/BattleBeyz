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

namespace Physics {
    constexpr float GRAVITY = 9.81f;
    constexpr glm::vec3 GRAVITY_VECTOR = glm::vec3(0.0f, -GRAVITY, 0.0f);

    // How much loss in angular speed is converted to linear speed due to friction
    constexpr float FRICTIONAL_EFFICIENCY = 0.5f;

    // How much impact the accelaration has on frictional force. Usually ranges from 0.1 to 0.5.
    // Follows a = g * mu * cos * direction vector (* FAC)
    constexpr float FRICTIONAL_ACCELERATION_CONSTANT = 0.17f;

    // How much impact we want the angular speed affects the frictional force with the stadium. Usually ranges from 0.01 to 0.1
    // Follows a = w * mu (* FVC)
    constexpr float FRICTIONAL_VELOCITY_CONSTANT = 0.04f;

    // How much aerial drag there is
    const float FLUID_DRAG = 0.08f;
    void accumulateAirResistance(BeybladeBody* beyblade, float airDensity = FLUID_DRAG);

    void accumulateFriction(BeybladeBody* beyblade, StadiumBody* stadium);
    void accumulateSlope(BeybladeBody* beyblade, StadiumBody* stadium);
    void accumulateImpact(BeybladeBody* beyblade1, BeybladeBody* beyblade2, float contactDistance);
    void preventStadiumClipping(BeybladeBody* beybladeBody, StadiumBody* stadiumBody);
}


