////////////////////////////////////////////////////////////////////////////////
// PhysicsWorld.cpp -- Core physics handling include -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Physics.h"
#include "Beyblade.h"
#include "Stadium.h"

#include "Units.h"
using namespace Units;

class GameEngine;
class ShaderProgram;

class PhysicsWorld {
public:
    // TODO: Move airDensityValue and other physicsal constants to Physics.h
    PhysicsWorld(Kg_M3 airDensityValue = 0.8_kg / (1.0_m*1.0_m*1.0_m), float spinThreshold = 30.0f) : airDensity(airDensityValue), SPIN_THRESHOLD(spinThreshold) {}

    void addBeyblade(Beyblade* body);
    void addStadium(Stadium* body);
    void removeBeyblade(Beyblade* body);
    void removeStadium(Stadium* body);

    void resetPhysics() { // 2024-11-18.  Clear before game restart.
        beyblades.clear();
        stadiums.clear();
        currTime = 0.0f;
        for (auto& bey : beyblades) bey->getRigidBody()->prevCollision = 0.0f;
    };

    void update(float deltaTime);
    void renderDebug(ShaderProgram &shader) const;

    std::vector<Beyblade*>& getBeyblades() { return beyblades; }
    std::vector<Stadium*>& getStadiums() { return stadiums; }

private:
    //float airDensity;
    float currTime = 0.0f;
    
    Kg_M3 airDensity;
    std::vector<Beyblade*> beyblades;
    std::vector<Stadium*> stadiums;
    const Scalar SPIN_THRESHOLD = 30.0__;
    const Scalar MAX_SPIN_THRESHOLD = 1500.0__;
    const Scalar PI = 3.14159265358979__;
    const float epsilonTime = 0.2f; // Cannot have collisions within 0.3 seconds of a previous one
};
