////////////////////////////////////////////////////////////////////////////////
// PhysicsWorld.cpp -- Core physics handling include -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <glm/glm.hpp>
#include <utility>

#include "BoundingBoxRenderer.h"
#include "Physics.h"
#include "Beyblade.h"
#include "Stadium.h"

class GameEngine;
class ObjectShader;

class PhysicsWorld {
public:
    PhysicsWorld(R_S minSpin = 30.0_rad_s, R_S maxSpin = 1500.0_rad_s, const Physics& physics = Physics())
        : physics(physics), MIN_SPIN_THRESHOLD(minSpin), MAX_SPIN_THRESHOLD(maxSpin) {
    }

    void addBeyblade(Beyblade* body);
    void addStadium(Stadium* body);
    void removeBeyblade(Beyblade* body);
    void removeStadium(Stadium* body);

    void setPhysics(Physics& p) {
        physics = std::move(p);
    }

    void resetPhysics() {
        accumulatorNs = 0;
        interpolationAlpha = 1.0f;
        for (auto& bey : beyblades) bey->getBody()->prevCollision = 0.0f;
        beyblades.clear();
        stadiums.clear();
        currTime = 0.0f;
        spinFinishedReported.clear();
        outOfBoundsReported.clear();
    };

    // Steps the world at a fixed rate regardless of frame rate, so behaviour does not
    // change with FPS and a hitch cannot produce one enormous dt. This is the only entry
    // point; the single fixed step it drives is private so no caller can bypass the clock.
    // Returns the number of fixed steps taken, which saturates at MAX_STEPS_PER_FRAME
    // when the simulation is falling behind real time.
    int advance(float frameDelta);

    // Fraction of a fixed step elapsed since the last one, for render interpolation
    float getInterpolationAlpha() const { return interpolationAlpha; }

    static constexpr float FIXED_DT = 1.0f / 240.0f;
    static constexpr long long FIXED_DT_NS = 1000000000LL / 240;
    static constexpr float MAX_FRAME_DELTA = 0.25f;   // Ignore stalls beyond this
    static constexpr int MAX_STEPS_PER_FRAME = 60;    // Never spiral trying to catch up
    void renderDebug(ObjectShader &shader) const;

    std::vector<Beyblade*>& getBeyblades() { return beyblades; }
    std::vector<Stadium*>& getStadiums() { return stadiums; }

private:
    void update(float deltaTime);

    Physics physics;

    std::vector<Beyblade*> beyblades;
    std::vector<Stadium*> stadiums;

    // Round-end conditions are logged on transition, not every frame
    std::unordered_set<BeybladeBody*> spinFinishedReported;
    std::unordered_set<BeybladeBody*> outOfBoundsReported;

    // Debug drawing only; mutable so renderDebug() can stay const
    mutable BoundingBoxRenderer boundingBoxRenderer;

    // Nanoseconds, not seconds: floating point accumulation makes the step count depend
    // on frame size, so the same elapsed time could yield a different number of steps.
    long long accumulatorNs = 0;
    float interpolationAlpha = 1.0f;

    float currTime = 0.0f;
    const float epsilonTime = 0.2f;                 // Cannot have collisions within this many seconds of a previous one
    const R_S MIN_SPIN_THRESHOLD = 30.0_rad_s;      // If a beyblade's |w| is less, the game ends due to spin finish
    const R_S MAX_SPIN_THRESHOLD = 1500.0_rad_s;    // Cannot launch higher than this speed
};
