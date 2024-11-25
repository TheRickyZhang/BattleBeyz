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

class PhysicsWorld {
public:
    // TODO: Move airDensityValue to Physics.h
    PhysicsWorld(double airDensityValue = 0.8, double spinThreshold = 30) : airDensity(airDensityValue), SPIN_THRESHOLD(spinThreshold) {}

    void addBeyblade(Beyblade* body);
    void addStadium(Stadium* body);
    void removeBeyblade(Beyblade* body);
    void removeStadium(Stadium* body);

    void update(float deltaTime);
    void renderDebug(ShaderProgram &shader) const;
#if 0
    std::vector<Beyblade*> getBeyblades() const { return beyblades; }
    std::vector<Stadium*> getStadiums() const { return stadiums; }
#else  // NEWUI
    std::vector<Beyblade*>& getBeyblades() { return beyblades; }
    std::vector<Stadium*>& getStadiums() { return stadiums; }
#endif

private:
    double airDensity;
    std::vector<Beyblade*> beyblades;
    std::vector<Stadium*> stadiums;
    const double SPIN_THRESHOLD = 30;
};
