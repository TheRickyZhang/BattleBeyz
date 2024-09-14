////////////////////////////////////////////////////////////////////////////////
// PhysicsWorld.cpp -- Core physics handling -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "PhysicsWorld.h"

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
* @param body               [in] A StadiumBody object
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

void PhysicsWorld::update(float deltaTime) {
    /**
    * Resolve bey-stadium collisions
    */
    for (Beyblade* beyblade : beyblades) {
        BeybladeBody* beybladeBody = beyblade->getRigidBody();
        // TODO: If bey is below minimum spin threshold, end the match. Some default animation could be used.
        if (glm::length(beybladeBody->getAngularVelocity()) < SPIN_THRESHOLD) {
            std::cerr << "Beyblade ran out of spin" << std::endl;
            return;
        }

        // Get position of the bottom tip
        glm::vec3 beyBottom = beybladeBody->getBottomPosition();

        // Should usually only be one stadium, but may need to scale to more
        for (Stadium* stadium : stadiums) {
            StadiumBody* stadiumBody = stadium->getRigidBody();
            if(!stadiumBody->isInside(beyBottom.x, beyBottom.z)) {
                // Game is over since beyblade is out of bounds, implement behavior in future
                std::cerr << "Beyblade out of bounds" << std::endl;
                return;
            }

            // Add air resistance
            Physics::accumulateAirResistance(beybladeBody, airDensity);

            double stadiumY = stadiumBody->getY(beyBottom.x, beyBottom.z);

            // If the Beyblade is airborne by some significant amount, only apply gravity
            if (beyBottom.y - stadiumY > 0.005) {
                beybladeBody->accumulateAcceleration(Physics::GRAVITY_VECTOR);
            }
            else {
                // Add friction and slope forces from contact
                Physics::accumulateFriction(beybladeBody, stadiumBody);
                Physics::accumulateSlope(beybladeBody, stadiumBody);
            }
        }
    }
    /**
    * Resolve bey-bey collisions
    */
    for (size_t i = 0; i < beyblades.size(); ++i) {
        for (size_t j = i + 1; j < beyblades.size(); ++j) {
            BeybladeBody* bey1 = beyblades[i]->getRigidBody();
            BeybladeBody* bey2 = beyblades[j]->getRigidBody();
            std::optional<double> contactDistance = BeybladeBody::distanceOverlap(bey1, bey2);

            // Skip beys with no contact
            if (!contactDistance.has_value()) continue;

            /**
            * Linear repulsive force combines the collision due to initial velocity with the recoil from spins
            * Angular draining force is the loss of spin of both beys due to colliding
            */
            Physics::accumulateImpact(bey1, bey2, contactDistance.value());
        }
    }

    /**
    * Apply forces simultaneously by storing them, rather than sequentially which can cause consistency issues
    * Then, apply all at once to change velocities, then update positions with new velocities.
    */
    for (Beyblade* beyblade : beyblades) {
        BeybladeBody* beybladeBody = beyblade->getRigidBody();
        beybladeBody->applyAccumulatedChanges(deltaTime);
        beybladeBody->update(deltaTime);
        for (Stadium* stadium : stadiums) {
            StadiumBody* stadiumBody = stadium->getRigidBody();
            // Prevent beyblade from ever clipping into the stadium during rendering
            Physics::preventStadiumClipping(beybladeBody, stadiumBody);
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

// TOFIX if not working?
void PhysicsWorld::renderDebug(ShaderProgram& shader) const {
    // Render all bounding boxes
    for (Beyblade* beyblade : beyblades) {
        BeybladeBody* beybladeBody = beyblade->getRigidBody();
        for (int i = 0; i < beybladeBody->boundingBoxes.size() /*&& i < 100*/; i++) {
            beybladeBody->boundingBoxes[i]->renderDebug(shader, beybladeBody->getCenter());
        }
    }
    for (Stadium* stadium : stadiums) {
        StadiumBody* stadiumBody = stadium->getRigidBody();
        for (int i = 0; i < stadiumBody->boundingBoxes.size() /*&& i < 100*/; i++) {
            stadiumBody->boundingBoxes[i]->renderDebug(shader, stadiumBody->getCenter());
        }
    }
}
