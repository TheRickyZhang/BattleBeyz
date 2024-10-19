////////////////////////////////////////////////////////////////////////////////
// Physics.cpp -- Physics code -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "Physics.h"

/**
* Calculate air resistance proportional to C * v^2 for both angular and linear components.
* 
* @param beyblade                   [in] Pointer to the beyblade body.
* 
* @param airDensity                 [in] Air density.
*/

void Physics::accumulateAirResistance(BeybladeBody* beyblade, double airDensity) {
    // b = 1/2 * Cd * A * p
    double linearDragConstant = beyblade->getLinearDragTerm() * airDensity;
    double velocityMagnitude = glm::length(beyblade->getVelocity());

    // Adrag = (-b * v^2 / mass) * unit v
    glm::vec3 linearAirResistanceAcceleration = -dv3(linearDragConstant * velocityMagnitude / beyblade->getMass()) * beyblade->getVelocity();

    // b = 1/2 * Cd * A * r^2 * p
    double angularDragConstant = beyblade->getAngularDragTerm() * airDensity;
    double angularVelocityMagnitude = glm::length(beyblade->getAngularVelocity());

    // Adrag = (-b * w^2 / moi) * unit w
    glm::vec3 angularAirResistanceAcceleration = -dv3(angularDragConstant * angularVelocityMagnitude / beyblade->getMomentOfInertia()) * beyblade->getAngularVelocity();

    beyblade->accumulateAngularAcceleration(angularAirResistanceAcceleration);
    beyblade->accumulateAcceleration(linearAirResistanceAcceleration);
}

/**
* Simulates the effects of beybldae-stadium friction contact. May be unintuitive if you are not familiar with Beyblades.
* 
* Applies a positive linear accelaration tangential to movement at driver-stadium contact.
* Applies a negative angular accelaration to oppose the beyblade's rotation.
* 
* The magnitude of accelaration is given by the sum of a traditional friction model and a rotation-speed dependent model.
* The relative strengths of these can be altered as parameters in Physics.
* 
* @param beyblade                   [in] Pointer to the beyblade body.
* 
* @param stadium                    [in] Pointer to the stadium body.
*/

void Physics::accumulateFriction(BeybladeBody* beyblade, StadiumBody* stadium) {
    // Gets the normal of the stadium at the beyblade's position
    glm::vec3 stadiumNormal = stadium->getNormal(beyblade->getCenter().x, beyblade->getCenter().z);

    // Unnecessary? Gets the rotation axis of the Bey (this may be different than the stadium)
    //glm::vec3 beybladeNormal = beyblade->getNormal();

    double combinedCOF = (stadium->getCOF() + beyblade->getDriverCOF()) / 2;
    glm::vec3 beyBottomPosition = beyblade->getBottomPosition();
    double stadiumY = stadium->getY(beyBottomPosition.x, beyBottomPosition.z);
    glm::vec3 normalizedAngularVelocity = glm::normalize(beyblade->getAngularVelocity());
 
    // sin(theta) * direction
    glm::vec3 frictionDirectionAcceleration = -glm::cross(normalizedAngularVelocity, stadiumNormal);
    double alignment = glm::dot(normalizedAngularVelocity, stadiumNormal);

    double linearComponent = Physics::GRAVITY * combinedCOF * alignment;
    double angularComponent = glm::length(beyblade->getAngularVelocity()) * combinedCOF * Physics::FRICTIONAL_VELOCITY_CONSTANT * alignment > 0 ? 1 : -1;

    // cl * (g * mu * cos(theta)
    double traditionalAccelerationComponent = Physics::FRICTIONAL_ACCELERATION_CONSTANT * linearComponent;

    // cv * (w * mu)
    double velocityAccelarationComponent = Physics::FRICTIONAL_VELOCITY_CONSTANT * angularComponent;

    // linear = (direction * sin(theta)) * (cl * (g * mu * cos(theta) + cv * (w * mu))
    glm::vec3 linearAcceleration = frictionDirectionAcceleration * dv3(traditionalAccelerationComponent + velocityAccelarationComponent);

    // Prevent case where Beyblade and Stadium are perfectly aligned, and nothing moves
    if (glm::length(linearAcceleration) < 0.001) { linearAcceleration = glm::vec3(0.001, 0, 0); }

    // angular = -direction * |linear| * mass * r / moi
    glm::vec3 angularAcceleration = -normalizedAngularVelocity * dv3(glm::length(linearAcceleration) * beyblade->getMass() * beyblade->getDriverRadius() / beyblade->getMomentOfInertia());

    beyblade->accumulateAcceleration(dv3(FRICTIONAL_EFFICIENCY) * linearAcceleration);
    beyblade->accumulateAngularAcceleration(angularAcceleration);
}

/**
* Apply a linear force to the Beyblade based on F = mu * m * g * cos(theta) * unit(displacement).
*
* @param beyblade                   [in] Pointer to the beyblade body.
*
* @param stadium                    [in] Pointer to the stadium body.
*/

void Physics::accumulateSlope(BeybladeBody* beyblade, StadiumBody* stadium)
{
    glm::vec3 beyBottomPosition = beyblade->getBottomPosition();
    glm::vec3 beybladeNormal = beyblade->getNormal();
    glm::vec3 stadiumNormal = stadium->getNormal(beyblade->getBottomPosition().x, beyblade->getBottomPosition().z);
    double combinedCOF = (stadium->getCOF() + beyblade->getDriverCOF()) / 2;

    if (glm::dot(stadiumNormal, beybladeNormal) <= 0.001f) {
        return;
    }
    glm::vec3 crossProduct = glm::cross(stadiumNormal, beybladeNormal);
    double sinOfAngle = glm::length(crossProduct) / (glm::length(stadiumNormal) * glm::length(beybladeNormal));

    // Check magnitudes here
    glm::vec3 unitDisplacement = glm::normalize(stadium->getCenter() - beyBottomPosition);
    glm::vec3 slopeForce = dv3(Physics::GRAVITY * sinOfAngle * combinedCOF) * unitDisplacement;
#if 0
    std::cout << Physics::GRAVITY << " " << sinOfAngle << " " << combinedCOF << std::endl;
    std::cout << unitDisplacement.x << " " << unitDisplacement.y << " " << unitDisplacement.z << std::endl;
    printVec3("Slope force", slopeForce);
#endif
    beyblade->accumulateAcceleration(slopeForce);
}

/**
* Calculates changes in velocity due to both linear and angular contact.
*
* @param beyblade1                  [in] Pointer to the first beyblade body.
*
* @param beyblade2                  [in] Pointer to the second beyblade body.
*
* @param contactDistance            [in] Contact distance from collision detection logic.
*/

void Physics::accumulateImpact(BeybladeBody* beyblade1, BeybladeBody* beyblade2, double contactDistance)
{
    // Goes from bey1 to bey2
    glm::vec3 center1Tocenter2 = beyblade2->getCenter() - beyblade1->getCenter();
    glm::vec3 unitSeparation = glm::normalize(center1Tocenter2);

    // Resolve clipping
    glm::vec3 displacement = dv3(0.5 * contactDistance) * unitSeparation;
    beyblade1->addCenterXZ(-displacement.x, -displacement.z);
    beyblade2->addCenterXZ(displacement.x, displacement.z);

    glm::vec3 velocity1 = beyblade1->getVelocity();
    glm::vec3 velocity2 = beyblade2->getVelocity();
    glm::vec3 vDiff = velocity2 - velocity1;
    double averageCOR = (beyblade1->getLayerCOR() + beyblade2->getLayerCOR()) / 2;

    double mass1 = beyblade1->getMass();
    double mass2 = beyblade2->getMass();

    // Trust this known linear collison model works correctly
    double impulseMagnitude = averageCOR * glm::dot(vDiff, unitSeparation) /
        (1.0 / mass1 + 1.0 / mass2);
    glm::vec3 impulse = dv3(impulseMagnitude) * unitSeparation;

    glm::vec3 deltaVelocity1 = dv3(-impulseMagnitude / mass1) * unitSeparation;
    glm::vec3 deltaVelocity2 = dv3(impulseMagnitude / mass2) * unitSeparation;

    // Need to set velocities directly, NOT accumulate them, since collision changes it instantaneously
    beyblade1->setVelocity(deltaVelocity1);
    beyblade2->setVelocity(deltaVelocity2);

    std::cout << "v1" << glm::length(velocity1) << " v2" << glm::length(velocity2) << std::endl;
    std::cout << "dv1" << glm::length(deltaVelocity1) << " dv2" << glm::length(deltaVelocity2) << std::endl;

    // Random effect with inherent attack power of beyblades built in
    double randomMagnitude = (beyblade1->sampleRecoil() + beyblade2->sampleRecoil());

    // Gets relative velocity moving towards each other.
    double linearCollisionSpeed = glm::dot(unitSeparation, velocity1) + glm::dot(-unitSeparation, velocity2);
    if (linearCollisionSpeed < 0) std::cerr << "Linear collision speed is less than 0!" << std::endl;

    // Different cases for same-spin vs opposite-spin collisions
    bool sameSpinDirection = beyblade1->isSpinningClockwise() == beyblade2->isSpinningClockwise();
    if (sameSpinDirection) {
        double angularSpeedDiff = beyblade1->getAngularVelocityMagnitude() + beyblade2->getAngularVelocityMagnitude();

        // Predictive modeling using y = log(x) - x/500 based on expected values for x.
        // Increases from AngularSpeedDiff = 0 to 500 and gently decreases from 500 to 1000, modeling empirical data (AKA I just made it up)
        double scalingFactor = 0.007 * (std::log(angularSpeedDiff + 1) - (angularSpeedDiff / 501)) * linearCollisionSpeed;

        // CHECK whether this conversion from angular to linear is correct.
        // Just simply multiply by COR since moment of inertia vs mass already accounts for?

        double recoilAngularImpulseMagnitude = randomMagnitude * scalingFactor;
        std::cerr << "Angular implulse magnitude (should be less than 0.001): " << recoilAngularImpulseMagnitude << std::endl;

        beyblade1->accumulateAngularImpulseMagnitude(-recoilAngularImpulseMagnitude);
        beyblade2->accumulateAngularImpulseMagnitude(-recoilAngularImpulseMagnitude);

        double recoilLinearImpulseMagnitude = recoilAngularImpulseMagnitude * averageCOR;

        beyblade1->accumulateImpulseMagnitude(-recoilLinearImpulseMagnitude);
        beyblade2->accumulateImpulseMagnitude(-recoilLinearImpulseMagnitude);
    }
    else {
        // TODO: Different case for opposite spin interactions
        double angularSpeedDiff = beyblade1->getAngularVelocityMagnitude() + beyblade2->getAngularVelocityMagnitude();
        std::cerr << "Opposite spin collisions have not been implemented yet";
    }
}

/**
* Prevent a blade from sinking into the stadium.
* 
* @param beybladeBody                   [in] Pointer to the blade body.
* 
* @param statidumBody                   [in] Pointer to the statidum body.
*/

void Physics::preventStadiumClipping(BeybladeBody* beybladeBody, StadiumBody* stadiumBody)
{
    glm::vec3 beyBottom = beybladeBody->getBottomPosition();
    double stadiumY = stadiumBody->getY(beyBottom.x, beyBottom.z);

    // Beyblade is clipping into stadium. Push it out along y-axis.
    if (stadiumY > beyBottom.y) {
        beybladeBody->addCenterY(stadiumY - beyBottom.y);
        beybladeBody->setVelocityY(0.0);
    }
}
