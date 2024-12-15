////////////////////////////////////////////////////////////////////////////////
// PhysicsWorld.cpp -- Core physics handling include -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Physics.h"
#include "Beyblade.h"
#include "Stadium.h"

#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>
#include "Utils.h"
#include "RigidBody.h"
#include "ShaderProgram.h"

#include "UnitsSystem.h"
using namespace Units;

class PhysicsWorld {
public:
    // TODO: Move airDensityValue to Physics.h
    PhysicsWorld(KilogramPerCubicMeter airDensityValue = 0.8_kg / (1.0_m*1.0_m*1.0_m), float spinThreshold = 30.0f) : airDensity(airDensityValue), SPIN_THRESHOLD(spinThreshold) {}

    void addBeyblade(Beyblade* body);
    void addStadium(Stadium* body);
    void removeBeyblade(Beyblade* body);
    void removeStadium(Stadium* body);

    void resetPhysics() { // 2024-11-18.  Clear before game restart.
        beyblades.clear();
        stadiums.clear();
    };

    void update(float deltaTime);
    void renderDebug(ShaderProgram &shader) const;

    std::vector<Beyblade*>& getBeyblades() { return beyblades; }
    std::vector<Stadium*>& getStadiums() { return stadiums; }

private:
    //float airDensity;
    KilogramPerCubicMeter airDensity;
    std::vector<Beyblade*> beyblades;
    std::vector<Stadium*> stadiums;
    const float SPIN_THRESHOLD = 30.0f;
    const Scalar PI = 3.14159265358979__;
};
