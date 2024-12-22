////////////////////////////////////////////////////////////////////////////////
// Physics.cpp -- Physics code -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "Physics.h"

using namespace std;
/**
* Calculate air resistance proportional to C * v^2 for both angular and linear components.
* 
* @param beyblade                   [in] Pointer to the beyblade body.
* 
* @param airDensity                 [in] Air density.
*/

void Physics::accumulateAirResistance(BeybladeBody* beyblade, Kg_M3 airDensity) {
    // c = 1/2 * Cd * A * p
    Kg_M linearDragConstant = beyblade->getLinearDragTerm() * airDensity;
    //Scalar velocityMagnitude = length(beyblade->getVelocity());

    auto res = dot(beyblade->getVelocity(), beyblade->getVelocity());
    // Adrag = (-b * v^2 / mass) * unit v
    Vec3_M_S2 linearAirResistanceAcceleration = -1.0f * linearDragConstant * dot(beyblade->getVelocity(), beyblade->getVelocity())
                                                            / beyblade->getMass() * normalize(beyblade->getVelocity());

    // b = 1/2 * Cd * A * r^3 * p
    KgM2 angularDragConstant = beyblade->getAngularDragTerm() * airDensity;
    //float angularVelocityMagnitude = glm::length(beyblade->getAngularVelocity());

    // Adrag = (-b * w^2 / moi) * unit w            (Note: must manually correct radian overcount here)
    Vec3_R_S2 angularAirResistanceAcceleration = -1.0__/R(1.0) * angularDragConstant * dot(beyblade->getAngularVelocity(), beyblade->getAngularVelocity())
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

void Physics::accumulateFriction(BeybladeBody* beyblade, StadiumBody* stadium) {
    // Gets the normal of the stadium at the beyblade's position
    Vec3_Scalar stadiumNormal = stadium->getNormal(beyblade->getCenter().xTyped(), beyblade->getCenter().zTyped());

    Scalar combinedCOF = (stadium->getCOF() + beyblade->getDriverCOF()) / 2.0__;
    Vec3_M beyBottomPosition = beyblade->getBottomPosition();
    M stadiumY = stadium->getY(beyBottomPosition.xTyped(), beyBottomPosition.zTyped());
    Vec3_Scalar normalizedAngularVelocity = normalize(beyblade->getAngularVelocity());
 
    // sin(theta) * direction.  Patched with units
    Vec3_Scalar frictionDirectionAcceleration = -cross(normalizedAngularVelocity, stadiumNormal);
    Scalar alignment = dot(normalizedAngularVelocity, stadiumNormal);

    // TODO: Represent radius as 1/rad units
    M_S2 linearComponent = Physics::GRAVITY * combinedCOF * alignment;
    M_S2 angularComponent = 1.0__/(1.0_s2) * (beyblade->getAngularVelocity().length() * beyblade->getDriverRadius()) * combinedCOF * (alignment > 0.0__ ? 1.0f : -1.0f);

    // cl * (g * mu * cos(theta)
    M_S2 traditionalAccelerationComponent = Physics::FRICTIONAL_ACCELERATION_CONSTANT * linearComponent;

    // cv * (w * mu)
    M_S2 velocityAccelarationComponent = Physics::FRICTIONAL_VELOCITY_CONSTANT * angularComponent;

    // linear = (direction * sin(theta)) * (cl * (g * mu * cos(theta) + cv * (w * mu))
    Vec3_M_S2 linearAcceleration = frictionDirectionAcceleration * (traditionalAccelerationComponent + velocityAccelarationComponent);

    // Prevent case where Beyblade and Stadium are perfectly aligned, and nothing moves
    //if (linearAcceleration.length() < 0.001__) {
    //    linearAcceleration = Vec3_M_S2(0.001, 0, 0);
    //}

    // angular = -direction * |linear| * mass * r / moi
    Vec3_R_S2 angularAcceleration = -1.0_rad * normalizedAngularVelocity *
        (linearAcceleration.lengthTyped() * beyblade->getMass() * beyblade->getDriverRadius() / beyblade->getMomentOfInertia());

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

void Physics::accumulateSlope(BeybladeBody* beyblade, StadiumBody* stadium)
{
    Vec3_M beyBottomPosition = beyblade->getBottomPosition();
    Vec3_Scalar beybladeNormal = beyblade->getNormal();
    Vec3_Scalar stadiumNormal = stadium->getNormal(beyblade->getBottomPosition().xTyped(), beyblade->getBottomPosition().zTyped());
    Scalar combinedCOF = (stadium->getCOF() + beyblade->getDriverCOF()) / 2.0__;

    Vec3_Scalar crossProduct = cross(stadiumNormal, beybladeNormal);
    Scalar sinOfAngle = crossProduct.length() / (stadiumNormal.length() * beybladeNormal.length());

    // Check magnitudes here
    Vec3_Scalar unitDisplacement = normalize(stadium->getCenter() - beyBottomPosition);
    Vec3_M_S2 slopeForce = (Physics::GRAVITY * sinOfAngle * combinedCOF) * unitDisplacement;
#if 0
    //cout << Physics::GRAVITY << " " << sinOfAngle << " " << combinedCOF << endl;
    //cout << unitDisplacement.x << " " << unitDisplacement.y << " " << unitDisplacement.z << endl;
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

// Checkpoint 12/16
void Physics::accumulateImpact(BeybladeBody* beyblade1, BeybladeBody* beyblade2, M contactDistance)
{
    // Goes from bey1 to bey2
    Vec3_M center1Tocenter2 = beyblade2->getCenter() - beyblade1->getCenter();
    Vec3_Scalar unitSeparation = normalize(center1Tocenter2);

    // Resolve clipping
    Vec3_M displacement = 0.5f * contactDistance * unitSeparation;
    beyblade1->addCenterXZ(-displacement.xTyped(), -displacement.zTyped());
    beyblade2->addCenterXZ(displacement.xTyped(), displacement.zTyped());

    auto d = BeybladeBody::distanceOverlap(beyblade1, beyblade2);
    if (d.has_value())  cout << "The distance between the beys is " << d.value() << endl;
    else cout << "The beys are not intersecting afterwards" << endl;

    Vec3_M_S velocity1 = beyblade1->getVelocity();
    Vec3_M_S velocity2 = beyblade2->getVelocity();
    Vec3_M_S vDiff = velocity2 - velocity1;
    Scalar averageCOR = (beyblade1->getLayerCOR() + beyblade2->getLayerCOR()) / 2.0__;

    Kg mass1 = beyblade1->getMass();
    Kg mass2 = beyblade2->getMass();

    M_S relativeSpeed = proj(vDiff, unitSeparation);
    cout << "projecting " << vDiff << " onto " << unitSeparation << "gives";
    cout << "relative speed " << relativeSpeed << endl;

    // Trust this known linear collison model works correctly
    KgM_S impulseMagnitude = averageCOR * relativeSpeed / (1.0__ / mass1 + 1.0__ / mass2);
    Vec3_KgM_S impulse = impulseMagnitude * unitSeparation;

    Vec3_M_S deltaVelocity1 = -impulseMagnitude / mass1 * unitSeparation;
    Vec3_M_S deltaVelocity2 = impulseMagnitude / mass2 * unitSeparation;

    // TODO URGENT PHYSICS: Ensure there is a cooldown after a collision to not have clipping multiple collisions
    // Need to set velocities directly, NOT accumulate them, since collision changes it instantaneously
    beyblade1->setVelocity(deltaVelocity1);
    beyblade2->setVelocity(deltaVelocity2);
    cout << "Bey1 velocity set to " << deltaVelocity1 << " when it was "<< velocity1<< endl;
    cout << "Bey2 velocity set to " << deltaVelocity2 << " when it was " << velocity2 << endl;

    cout << fixed << setprecision(5);
    cout << "v1: " << glm::length(velocity1.value()) << " | v2: " << glm::length(velocity2.value()) << endl;
    cout << "dv1: " << glm::length(deltaVelocity1.value()) << " | dv2: " << glm::length(deltaVelocity2.value()) << endl;

    // Random effect with inherent attack power of beyblades built in
    Scalar randomMagnitude = (beyblade1->sampleRecoil() + beyblade2->sampleRecoil()) / 2.0__;
    assert(randomMagnitude > 0.0__);

    // NOTE: I think this is the same as relativeSpeed but with reversed sign.
    
    //float linearCollisionSpeed = glm::dot(unitSeparation, velocity1) + glm::dot(-unitSeparation, velocity2);
    //if (linearCollisionSpeed < 0) cerr << "Linear collision speed is less than 0!" << endl;

    KgM2 averageMOI = (beyblade1->getMomentOfInertia() + beyblade2->getMomentOfInertia()) / 2.0__;
    Kg averageMass = (mass1 + mass2) / 2.0__;
    // Different cases for same-spin vs opposite-spin collisions
    bool sameSpinDirection = beyblade1->isSpinningClockwise() == beyblade2->isSpinningClockwise();
    if (sameSpinDirection) {
        Scalar angularSpeedDiff = beyblade1->getAngularVelocity().length() + beyblade2->getAngularVelocity().length();
        cerr << fixed << setprecision(5) << "Angular speed diff " << endl;

        // TODO: More accurate predictive modeling, use sqrt() for now
        // NOTE we assume magnitude bounded by MIN and MAX spin threshold
        Scalar angularScalingFactor = 1.0__ * sqrt(abs(relativeSpeed.value())) * sqrt(angularSpeedDiff.value());
        Scalar linearScalingFactor = 0.2__ * sqrt(abs(relativeSpeed.value()))
            * (1.0f + float((std::clamp(angularSpeedDiff.value(), 30.0f, 1500.0f) - 30.0f)
                * (10.0 - 1.0) / (1500.0f - 30.0f)));

        // CHECK whether this conversion from angular to linear is correct.
        // Just simply multiply by COR since moment of inertia vs mass already accounts for?

        Scalar recoilAngularImpulseMagnitude(randomMagnitude * angularScalingFactor);  // Since we can't simulate directly fudge up units
        cerr << fixed << setprecision(5) << "Angular implulse magnitude (< 0.001): " << recoilAngularImpulseMagnitude.value() << endl;
        assert(recoilAngularImpulseMagnitude.value() > 0.0);

        beyblade1->accumulateAngularImpulseMagnitude(-1.0__/1.0_s * recoilAngularImpulseMagnitude * averageMOI);
        beyblade2->accumulateAngularImpulseMagnitude(-1.0__/1.0_s * recoilAngularImpulseMagnitude * averageMOI);

        M_S recoilLinearImpulseMagnitude(randomMagnitude * linearScalingFactor * averageCOR);
        cerr << fixed << setprecision(5) << "Linear implulse magnitude (< 0.02): " << recoilLinearImpulseMagnitude.value() << endl;
        assert(recoilLinearImpulseMagnitude.value() > 0.0);

        beyblade1->accumulateImpulseMagnitude(-recoilLinearImpulseMagnitude * averageMass);
        beyblade2->accumulateImpulseMagnitude(-recoilLinearImpulseMagnitude * averageMass);
    }
    else {
        // TODO: Different case for opposite spin interactions
        auto angularSpeedDiff = beyblade1->getAngularVelocity().length() + beyblade2->getAngularVelocity().length();
        cerr << "Opposite spin collisions have not been implemented yet";
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
    Vec3_M beyBottom = beybladeBody->getBottomPosition();
    M stadiumY = stadiumBody->getY(beyBottom.xTyped(), beyBottom.zTyped());

    // Beyblade is clipping into stadium. Push it out along y-axis.
    if (stadiumY > beyBottom.yTyped()) {
        beybladeBody->addCenterY(stadiumY - beyBottom.yTyped());
        beybladeBody->setVelocityY(0.0_m_s);
    }
}
