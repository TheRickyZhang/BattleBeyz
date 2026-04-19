////////////////////////////////////////////////////////////////////////////////
// Physics.cpp -- Physics code -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include <iomanip>
#include <sstream>
#include <algorithm>

#include "Physics.h"

#include "BeybladeBody.h"
#include "MessageLog.h"
#include "Stadium.h"

using namespace std;
/**
* Calculate air resistance proportional to C * v^2 for both angular and linear components.
* 
* @param beyblade                   [in] Pointer to the beyblade body.
* 
* @param airDensity                 [in] Air density.
*/

void Physics::accumulateAirResistance(BeybladeBody* beyblade) const {
    // c = 1/2 * Cd * A * p
    Kg_M linearDragConstant = beyblade->getLinearDragTerm() * FLUID_DRAG;
    // Adrag = (-b * v^2 / mass) * unit v
    Vec3_M_S2 linearAirResistanceAcceleration = -1.0f * linearDragConstant * dot(beyblade->getVelocity(), beyblade->getVelocity())
                                                            / beyblade->getMass() * normalize(beyblade->getVelocity());

    // b = 1/2 * Cd * A * r^3 * p
    KgM2 angularDragConstant = beyblade->getAngularDragTerm() * FLUID_DRAG;
    //float angularVelocityMagnitude = glm::length(beyblade->getAngularVelocity());

    // Adrag = (-b * w^2 / moi) * unit w            (Note: must manually correct radian overcount here)
    Vec3_R_S2 angularAirResistanceAcceleration = -1.0_sc/R(1.0) * angularDragConstant * dot(beyblade->getAngularVelocity(), beyblade->getAngularVelocity())
                                            / beyblade->getMomentOfInertia() * normalize(beyblade->getAngularVelocity());

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

void Physics::accumulateFriction(BeybladeBody* beyblade, Stadium* stadium) const {
    // Gets the normal of the stadium at the beyblade's position
    Vec3_Scalar stadiumNormal = stadium->getNormal(beyblade->getCenter().xTyped(), beyblade->getCenter().zTyped());

    Scalar combinedCOF = (stadium->getCOF() + beyblade->driver->coefficientOfFriction) / 2.0_sc;
    Vec3_Scalar normalizedAngularVelocity = normalize(beyblade->getAngularVelocity());
 
    // sin(theta) * direction.  Patched with units
    Vec3_Scalar frictionDirectionAcceleration = -cross(normalizedAngularVelocity, stadiumNormal);
    Scalar alignment = dot(normalizedAngularVelocity, stadiumNormal);

    M_S2 linearComponent = GRAVITY * combinedCOF * alignment;
    // w*r is a speed (rad cancels); dividing by one radian-second turns it into the
    // acceleration this model wants, without silently dropping the units on the way.
    M_S2 angularComponent = (beyblade->getAngularVelocity().lengthTyped() * beyblade->driver->contactRadius)
        / (1.0_rad * 1.0_s) * combinedCOF * (alignment > 0.0_sc ? 1.0f : -1.0f);

    // cl * (g * mu * cos(theta)
    M_S2 traditionalAccelerationComponent = FRICTIONAL_ACCELERATION_CONSTANT * linearComponent;

    // cv * (w * mu)
    M_S2 velocityAccelarationComponent = FRICTIONAL_VELOCITY_CONSTANT * angularComponent;

    // linear = (direction * sin(theta)) * (cl * (g * mu * cos(theta) + cv * (w * mu))
    Vec3_M_S2 linearAcceleration = frictionDirectionAcceleration * (traditionalAccelerationComponent + velocityAccelarationComponent);

    // Prevent case where Beyblade and Stadium are perfectly aligned, and nothing moves
    //if (linearAcceleration.length() < 0.001_sc) {
    //    linearAcceleration = Vec3_M_S2(0.001, 0, 0);
    //}

    // angular = -direction * |linear| * mass * r / moi
    Vec3_R_S2 angularAcceleration = -1.0_rad * normalizedAngularVelocity *
        (linearAcceleration.lengthTyped() * beyblade->getMass() * beyblade->driver->contactRadius / beyblade->getMomentOfInertia());

    beyblade->accumulateAcceleration(FRICTIONAL_EFFICIENCY * linearAcceleration);
    beyblade->accumulateAngularAcceleration(angularAcceleration);
}

/**
* Apply a linear force to the Beyblade based on F = mu * m * g * cos(theta) * unit(displacement).
*
* @param beyblade                   [in] Pointer to the beyblade body.
*
* @param stadium                    [in] Pointer to the stadium body.
*/

void Physics::accumulateSlope(BeybladeBody* beyblade, Stadium* stadium) const
{
    Vec3_M beyBottomPosition = beyblade->getBottomPosition();
    Vec3_Scalar beybladeNormal = beyblade->getNormal();
    Vec3_Scalar stadiumNormal = stadium->getNormal(beyblade->getBottomPosition().xTyped(), beyblade->getBottomPosition().zTyped());
    Scalar combinedCOF = (stadium->getCOF() + beyblade->driver->coefficientOfFriction) / 2.0_sc;

    Vec3_Scalar crossProduct = cross(stadiumNormal, beybladeNormal);
    Scalar sinOfAngle = crossProduct.length() / (stadiumNormal.length() * beybladeNormal.length());

    // Check magnitudes here
    Vec3_Scalar unitDisplacement = normalize(stadium->getCenter() - beyBottomPosition);
    Vec3_M_S2 slopeForce = (GRAVITY * sinOfAngle * combinedCOF) * unitDisplacement;

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

void Physics::accumulateImpact(BeybladeBody* beyblade1, BeybladeBody* beyblade2, M contactDistance)
{
    // Goes from bey1 to bey2
    Vec3_M center1Tocenter2 = beyblade2->getCenter() - beyblade1->getCenter();
    Vec3_Scalar unitSeparation = normalize(center1Tocenter2);

    // Resolve clipping
    Vec3_M displacement = 0.5f * contactDistance * unitSeparation;
    beyblade1->addCenterXZ(-displacement.xTyped(), -displacement.zTyped());
    beyblade2->addCenterXZ(displacement.xTyped(), displacement.zTyped());

    Vec3_M_S velocity1 = beyblade1->getVelocity();
    Vec3_M_S velocity2 = beyblade2->getVelocity();
    Vec3_M_S vDiff = velocity2 - velocity1;
    Scalar averageCOR = (beyblade1->layer->coefficientOfRestitution + beyblade2->layer->coefficientOfRestitution) / 2.0_sc;

    Kg mass1 = beyblade1->getMass();
    Kg mass2 = beyblade2->getMass();

    M_S relativeSpeed = proj(vDiff, unitSeparation);

    // Trust this known linear collison model works correctly
    KgM_S impulseMagnitude = averageCOR * relativeSpeed / (1.0_sc / mass1 + 1.0_sc / mass2);

    Vec3_M_S deltaVelocity1 = -impulseMagnitude / mass1 * unitSeparation;
    Vec3_M_S deltaVelocity2 = impulseMagnitude / mass2 * unitSeparation;

    // Need to set velocities directly, NOT accumulate them, since collision changes it instantaneously
    beyblade1->setVelocity(deltaVelocity1);
    beyblade2->setVelocity(deltaVelocity2);

    ostringstream collision;
    collision << fixed << setprecision(5)
        << "Impact: relative speed " << relativeSpeed.value()
        << " | v1 " << glm::length(velocity1.value()) << " -> " << glm::length(deltaVelocity1.value())
        << " | v2 " << glm::length(velocity2.value()) << " -> " << glm::length(deltaVelocity2.value());
    MessageLog::getInstance().addMessage(collision.str(), MessageType::Normal);

    // Random effect with inherent attack power of beyblades built in
    Scalar randomMagnitude = (beyblade1->sampleRecoil() + beyblade2->sampleRecoil()) / 2.0_sc;
    assert(randomMagnitude > 0.0_sc);

    // NOTE: I think this is the same as relativeSpeed but with reversed sign.
    
    //float linearCollisionSpeed = glm::dot(unitSeparation, velocity1) + glm::dot(-unitSeparation, velocity2);
    //if (linearCollisionSpeed < 0) cerr << "Linear collision speed is less than 0!" << endl;

    KgM2 averageMOI = (beyblade1->getMomentOfInertia() + beyblade2->getMomentOfInertia()) / 2.0_sc;
    Kg averageMass = (mass1 + mass2) / 2.0_sc;
    // Different cases for same-spin vs opposite-spin collisions
    bool sameSpinDirection = beyblade1->isSpinningClockwise() == beyblade2->isSpinningClockwise();
    if (sameSpinDirection) {
        // Both spin the same way, so the magnitudes add rather than cancel
        R_S angularSpeedSum = beyblade1->getAngularVelocity().lengthTyped() + beyblade2->getAngularVelocity().lengthTyped();

        // TODO: More accurate predictive modeling, use sqrt() for now
        // NOTE we assume magnitude bounded by MIN and MAX spin threshold
        Scalar angularScalingFactor = 1.0_sc * sqrt(abs(relativeSpeed.value())) * sqrt(angularSpeedSum.value());
        Scalar linearScalingFactor = 0.2_sc * sqrt(abs(relativeSpeed.value()))
            * (1.0f + float((std::clamp(angularSpeedSum.value(), 30.0f, 1500.0f) - 30.0f)
                * (10.0 - 1.0) / (1500.0f - 30.0f)));

        // CHECK whether this conversion from angular to linear is correct.
        // Just simply multiply by COR since moment of inertia vs mass already accounts for?

        Scalar recoilAngularImpulseMagnitude(randomMagnitude * angularScalingFactor);  // Since we can't simulate directly fudge up units
        assert(recoilAngularImpulseMagnitude.value() > 0.0);

        beyblade1->accumulateAngularImpulseMagnitude(-1.0_sc/1.0_s * recoilAngularImpulseMagnitude * averageMOI);
        beyblade2->accumulateAngularImpulseMagnitude(-1.0_sc/1.0_s * recoilAngularImpulseMagnitude * averageMOI);

        M_S recoilLinearImpulseMagnitude(randomMagnitude * linearScalingFactor * averageCOR);
        assert(recoilLinearImpulseMagnitude.value() > 0.0);

        beyblade1->accumulateImpulseMagnitude(-recoilLinearImpulseMagnitude * averageMass);
        beyblade2->accumulateImpulseMagnitude(-recoilLinearImpulseMagnitude * averageMass);
    }
    else {
        // TODO: Different case for opposite spin interactions
        MessageLog::getInstance().addMessage("Opposite spin collisions have not been implemented yet", MessageType::Warning);
    }
}

/**
* Prevent a blade from sinking into the stadium.
* 
* @param beybladeBody                   [in] Pointer to the blade body.
* 
* @param statidumBody                   [in] Pointer to the statidum body.
*/

void Physics::preventStadiumClipping(BeybladeBody* beybladeBody, Stadium* stadium)
{
    Vec3_M beyBottom = beybladeBody->getBottomPosition();
    M stadiumY = stadium->getY(beyBottom.xTyped(), beyBottom.zTyped());

    // Beyblade is clipping into stadium. Push it out along y-axis.
    if (stadiumY > beyBottom.yTyped()) {
        beybladeBody->addCenterY(stadiumY - beyBottom.yTyped());
        beybladeBody->setVelocityY(0.0_m_s);
    }
}
