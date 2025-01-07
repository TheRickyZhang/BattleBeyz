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

class GameEngine;
class ObjectShader;

class PhysicsWorld {
public:
    PhysicsWorld(Scalar minSpin = 30.0__, Scalar maxSpin = 1500.0__, const Physics& physics = Physics())
        : MIN_SPIN_THRESHOLD(minSpin), MAX_SPIN_THRESHOLD(maxSpin), physics(physics) {
    }

    void addBeyblade(Beyblade* body);
    void addStadium(Stadium* body);
    void removeBeyblade(Beyblade* body);
    void removeStadium(Stadium* body);

    void resetPhysics() {
        beyblades.clear();
        stadiums.clear();
        currTime = 0.0f;
        for (auto& bey : beyblades) bey->getRigidBody()->prevCollision = 0.0f;
    };

    void update(float deltaTime);
    void renderDebug(ObjectShader &shader) const;

    std::vector<Beyblade*>& getBeyblades() { return beyblades; }
    std::vector<Stadium*>& getStadiums() { return stadiums; }

private:
    Physics physics;

    std::vector<Beyblade*> beyblades;
    std::vector<Stadium*> stadiums;

    float currTime = 0.0f;
    const float epsilonTime = 0.2f;                 // Cannot have collisions within 0.3 seconds of a previous one
    const Scalar MIN_SPIN_THRESHOLD = 30.0__;       // If a beyblade's |w| is less, the game ends due to spin finish
    const Scalar MAX_SPIN_THRESHOLD = 1500.0__;     // Cannot launch higher than this speed
};
