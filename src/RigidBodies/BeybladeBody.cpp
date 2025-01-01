#include "BeybladeBody.h"
#include "BoundingBox.h"

BeybladeBody::BeybladeBody(Layer layer, Disc disc, Driver driver) :
    layer(layer), disc(disc), driver(driver),
    mass(layer.mass + disc.mass + driver.mass),
    momentOfInertia(layer.momentOfInertia + disc.momentOfInertia + driver.momentOfInertia)
{   
    // Used to be older physics code here for the very annoying drag terms to get correct. See GitHub history if still needed.
    
    // Sum 0.5 * Cd * A = 0.5 * Cd * 2 * r =
    // Cd * r * h
    M2 linearLayerCA = 0.9f * layer.radius * layer.height;
    M2 linearDiscCA = 0.9f * disc.radius * disc.height;
    M2 linearDriverCA = 0.9f * driver.radius * driver.height;
    linearDragTerm = linearLayerCA + linearDiscCA + linearDriverCA;

    // Sum 1/2 Ca * A * r^2 = 1/2 Ca * (2pi * r * h) * r^2 =
    // Cd * pi * r^3 * h
    M5 angularLayerCAr2 = layer.rotationalDragCoefficient * PI * pow<4>(layer.radius) * layer.height;
    M5 angularDiscCAr2 = disc.rotationalDragCoefficient * PI * pow<4>(disc.radius) * disc.height;
    M5 angularDriverCAr2 = driver.rotationalDragCoefficient * PI * pow<4>(driver.radius) * driver.height;
    angularDragTerm = angularLayerCAr2 + angularDiscCAr2 + angularDriverCAr2;

    modified = false;  // 2024-12-03
}

BeybladeBody::BeybladeBody() : BeybladeBody(Layer(), Disc(), Driver()) {}


/*--------------------------------------------Specialized Getters--------------------------------------------*/
Vec3_Scalar BeybladeBody::getNormal() const {
    Vec3_Scalar normalizedAngularVelocity(normalize(angularVelocity));
    // If the y-component is negative, reverse the vector
    if (normalizedAngularVelocity.y() < 0) {
        normalizedAngularVelocity = -normalizedAngularVelocity;
    }
    return normalizedAngularVelocity;
}

Vec3_M BeybladeBody::getBottomPosition() const {
    Vec3_Scalar unitDown = Vec3_Scalar(angularVelocity.y() < 0 ? normalize(angularVelocity) : -normalize(angularVelocity));
    Vec3_M tiltedDisplacement = (disc.height + driver.height) * unitDown;
    Vec3_M bottomPosition = baseCenter + tiltedDisplacement;
    return bottomPosition;
}

/**
* Reset physics data before start of new game.
*/

void BeybladeBody::resetPhysics()
{
    // Added 2024-11-18
    baseCenter = _initialBaseCenter;
    velocity = _initialVelocity;
    angularVelocity = _initialAngularVelocity;

    accumulatedAcceleration = Vec3_M_S2(0.0, 0.0, 0.0);
    accumulatedAngularAcceleration = Vec3_R_S2(0.0, 0.0, 0.0);
    accumulatedVelocity = Vec3_M_S(0.0, 0.0, 0.0);

    prevCollision = 0.0f;
}

void BeybladeBody::setInitialLaunch(Vec3_M initialCenter, Vec3_M_S initialVelocity, Vec3_R_S initialAngularVelocity)
{
    baseCenter = initialCenter;
    velocity = initialVelocity;
    angularVelocity = initialAngularVelocity;

    // Save some values for restart.  2024-11-18

    _initialBaseCenter = baseCenter;
    _initialVelocity = velocity;
    _initialAngularVelocity = angularVelocity;
}

// IMPROVE: Gets rough bounding box based on current dimensions
BoundingBox BeybladeBody::getBoundingBox() const {
    glm::vec3 mx = baseCenter.value() + glm::vec3(layer.radius.value(), layer.height.value(), layer.radius.value());
    glm::vec3 mn = baseCenter.value() - glm::vec3(layer.radius.value(), disc.height.value() + driver.height.value(), layer.radius.value());
    return BoundingBox(mx, mn);
}

/*--------------------------------------------Collision Calculations--------------------------------------------*/

Scalar BeybladeBody::sampleRecoil()
{
    return layer.recoilDistribution.sample();
}

std::optional<M> BeybladeBody::distanceOverlap(BeybladeBody* a, BeybladeBody* b) {
    if (!a || !b) {
        throw std::invalid_argument("Null pointer created in Beyblade::inContact");
    }

    Vec3_M aCenter = a->getCenter();
    Vec3_M bCenter = b->getCenter();

    BeybladeBody* lowerBey = aCenter.y() < bCenter.y() ? a : b;
    BeybladeBody* higherBey = lowerBey == a ? b : a;

    // Return nothing (no contact) early if layers do not vertically overlap
    if (lowerBey->getCenter().yTyped() + lowerBey->getLayerHeight() < higherBey->getCenter().yTyped()) {
        return std::nullopt;
    }

    M diffX = aCenter.xTyped() - bCenter.xTyped();
    M diffZ = aCenter.zTyped() - bCenter.zTyped();
    M2 squaredDistance = diffX * diffX + diffZ * diffZ;
    M radiiSum = a->getLayerRadius() + b->getLayerRadius();

    M2 overlapDistance = radiiSum * radiiSum - squaredDistance;

    // Checks for horizontal overlap based on xz coordinates with radii
    if (overlapDistance > 0.0_m2) {
        return std::optional<M>(root<2>(overlapDistance));
    }
    return std::nullopt;
}

/*--------------------------------------------Accumulators--------------------------------------------*/

void BeybladeBody::accumulateVelocity(Vec3_M_S addedVelocity)
{
    accumulatedVelocity += addedVelocity;
}

void BeybladeBody::accumulateAngularVelocity(Vec3_R_S addedAngularVelocity)
{
    accumulatedAngularVelocity += addedAngularVelocity;
}

void BeybladeBody::accumulateAcceleration(Vec3_M_S2 addedAcceleration)
{
    accumulatedAcceleration += addedAcceleration;
}

void BeybladeBody::accumulateAngularAcceleration(Vec3_R_S2 addedAngularAcceleration)
{
    accumulatedAngularAcceleration += addedAngularAcceleration;
}


/*--------------------------------------------Updators--------------------------------------------*/

// Increases or decreases linear speed given linear impulse magnitude
void BeybladeBody::accumulateImpulseMagnitude(KgM_S magnitude)
{
    Vec3_KgM_S deltaImpulse = magnitude * normalize(velocity);
    Vec3_M_S deltaVelocity = deltaImpulse / mass;
    accumulateVelocity(deltaVelocity);
}

// Increases or decreases angular speed given angular impulse magnitude
void BeybladeBody::accumulateAngularImpulseMagnitude(KgM2_S magnitude)
{
    Vec3_KgM2_S deltaAngularImpulse = magnitude * normalize(angularVelocity);
    Vec3_R_S deltaAngularVelocity = 1.0_rad * deltaAngularImpulse / momentOfInertia;
    accumulateAngularVelocity(deltaAngularVelocity);
}


void BeybladeBody::applyAccumulatedChanges(float deltaTime)
{
#if 0
    printVec3("Accumulated velocity", accumulatedVelocity);
    printVec3("Accumulated angular velocity", accumulatedAngularVelocity);
    printVec3("Accumulated acceleration", accumulatedAcceleration);
    printVec3("Accumulated angular acceleration", accumulatedAngularAcceleration);
#endif

    velocity += accumulatedVelocity;
    angularVelocity += accumulatedAngularVelocity;

    velocity += accumulatedAcceleration * S(deltaTime);
    angularVelocity += accumulatedAngularAcceleration * S(deltaTime);

    accumulatedVelocity.reset();
    accumulatedAngularVelocity.reset();
    accumulatedAcceleration.reset();
    accumulatedAngularAcceleration.reset();
}

void BeybladeBody::update(float deltaTime)
{
    baseCenter += velocity * S(deltaTime);
#if 0
    std::cout << "Base center: " << baseCenter.x << ", " << baseCenter.y << ", " << baseCenter.z << std::endl;
#endif
}