////////////////////////////////////////////////////////////////////////////////
// PhysicsWorld.cpp -- Core physics handling -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <algorithm>
#include <cmath>

#include "PhysicsWorld.h"

#include "ObjectShader.h"
#include "MessageLog.h"

/**
* Add a beyblade body to the scene.
* 
* @param body               [in] A BeybladeBody object
*/

void PhysicsWorld::addBeyblade(Beyblade* body) {
    beyblades.push_back(body);
}

/**
* Add a stadium body to the scene.
*
* @param body               [in] A Stadium object
*/

void PhysicsWorld::addStadium(Stadium* body) {
    stadiums.push_back(body);
}



void PhysicsWorld::removeBeyblade(Beyblade* body) {
    beyblades.erase(std::remove(beyblades.begin(), beyblades.end(), body), beyblades.end());
}

void PhysicsWorld::removeStadium(Stadium* body) {
    stadiums.erase(std::remove(stadiums.begin(), stadiums.end(), body), stadiums.end());
}

/**
* Update beyblade physics state in main physics loop.
*
* @param deltaTime              [in] Time increment in seconds.
*/

int PhysicsWorld::advance(float frameDelta) {
    // A long stall (window drag, a slow load) must not turn into hundreds of steps
    accumulatorNs += std::llround(std::min(frameDelta, MAX_FRAME_DELTA) * 1000000000.0);

    int steps = 0;
    while (accumulatorNs >= FIXED_DT_NS && steps < MAX_STEPS_PER_FRAME) {
        update(FIXED_DT);
        accumulatorNs -= FIXED_DT_NS;
        ++steps;
    }

    // Still behind after the cap: drop the debt rather than spiral
    if (steps == MAX_STEPS_PER_FRAME) accumulatorNs = 0;

    interpolationAlpha = static_cast<float>(accumulatorNs) / static_cast<float>(FIXED_DT_NS);
    return steps;
}

// TODO: Handle game logic when round is over
void PhysicsWorld::update(float deltaTime) {
    extern void UISetRunState(bool isError, const std::string & msg);  // Defined in UI.h

    currTime += deltaTime;
    /**
    * Resolve bey-stadium collisions
    */
    for (Beyblade* beyblade : beyblades) {
        BeybladeBody* beybladeBody = beyblade->getBody();
        if (beybladeBody->getAngularVelocity().lengthTyped() < MIN_SPIN_THRESHOLD) {
            if (spinFinishedReported.insert(beybladeBody).second) {
                MessageLog::getInstance().addMessage("Beyblade " + beyblade->getName() + " ran out of spin", MessageType::Normal);
            }
            continue;  // This bey is done; the rest of the world keeps stepping
        }
        spinFinishedReported.erase(beybladeBody);

        // Get position of the bottom tip
        Vec3_M beyBottom = beybladeBody->getBottomPosition();

        // Should usually only be one stadium, but may need to scale to more
        for (Stadium* stadium : stadiums) {
            if(!stadium->isInside(beyBottom.xTyped(), beyBottom.zTyped())) {
                if (outOfBoundsReported.insert(beybladeBody).second) {
                    MessageLog::getInstance().addMessage("Beyblade " + beyblade->getName() + " out of bounds", MessageType::Normal);
                }
                continue;  // Not in this stadium; check any others
            }
            outOfBoundsReported.erase(beybladeBody);

            // Add air resistance
            physics.accumulateAirResistance(beybladeBody);

            M stadiumY = stadium->getY(beyBottom.xTyped(), beyBottom.zTyped());

            // If the Beyblade is airborne by some significant amount, only apply gravity
            if (beyBottom.yTyped() - stadiumY > 0.005_m) {
                beybladeBody->accumulateAcceleration(physics.GRAVITY_VECTOR);
            }
            else {
                // Add friction and slope forces from contact
                physics.accumulateFriction(beybladeBody, stadium);
                physics.accumulateSlope(beybladeBody, stadium);
            }
        }
    }
    /**
    * Resolve bey-bey collisions
    */
    for (size_t i = 0; i < beyblades.size(); ++i) {
        for (size_t j = i + 1; j < beyblades.size(); ++j) {
            BeybladeBody* bey1 = beyblades[i]->getBody();
            BeybladeBody* bey2 = beyblades[j]->getBody();
            std::optional<M> contactDistance = BeybladeBody::distanceOverlap(bey1, bey2);

            // Skip beys with no contact
            if (!contactDistance.has_value()) continue;
            if (currTime - bey1->prevCollision < epsilonTime || currTime - bey2->prevCollision < epsilonTime) {
                continue;
            }

            /**
            * Linear repulsive force combines the collision due to initial velocity with the recoil from spins
            * Angular draining force is the loss of spin of both beys due to colliding
            */
            physics.accumulateImpact(bey1, bey2, contactDistance.value());
            bey1->prevCollision = bey2->prevCollision = currTime;
        }
    }

    /**
    * Apply forces simultaneously by storing them, rather than sequentially which can cause consistency issues
    * Then, apply all at once to change velocities, then update positions with new velocities.
    */

    for (Beyblade* beyblade : beyblades) {
        BeybladeBody* beybladeBody = beyblade->getBody();
        beybladeBody->applyAccumulatedChanges(deltaTime);
        beybladeBody->update(deltaTime);
        for (Stadium* stadium : stadiums) {
            // Prevent beyblade from ever clipping into the stadium during rendering
            physics.preventStadiumClipping(beybladeBody, stadium);
        }
    }
}

/**
* Debug render.
* 
* This currently shows all of the bounding boxes.  You can generate additional
* debug here.
* 
* @param shader                 [in] Our custom ShaderProgram.
*/

// Limited to 100 per object otherwise FPS tanks
void PhysicsWorld::renderDebug(ObjectShader& shader) const {
    boundingBoxRenderer.begin(shader);

    // Render all bounding boxes
    for (Beyblade* beyblade : beyblades) {
        BeybladeBody* beybladeBody = beyblade->getBody();
        for (size_t i = 0; i < beybladeBody->boundingBoxes.size() && i < 100; i++) {
            boundingBoxRenderer.draw(shader, beybladeBody->boundingBoxes[i], beybladeBody->getCenter().value());
        }
    }

    for (Stadium* stadium : stadiums) {
        for (size_t i = 0; i < stadium->boundingBoxes.size() && i < 100; i++) {
            boundingBoxRenderer.draw(shader, stadium->boundingBoxes[i], stadium->getCenter().value());
        }
    }

    boundingBoxRenderer.end();
}
