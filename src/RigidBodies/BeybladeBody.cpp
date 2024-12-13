#include "BeybladeBody.h"

BeybladeBody::BeybladeBody(Layer layer, Disc disc, Driver driver) :
    layer(layer), disc(disc), driver(driver),
    mass(layer.mass + disc.mass + driver.mass),
    momentOfInertia(layer.momentOfInertia + disc.momentOfInertia + driver.momentOfInertia)
{
    // Sum 0.5 * Cd * A for each part
    // Use 0.9 for now using cylindrical approximation
    float linearLayerCA = 0.9f * 2 * layer.height * layer.radius;
    float linearDiscCA = 0.9f * 2 * disc.height * disc.radius;
    float linearDriverCA = 0.9f * driver.height * driver.radius;
    linearDragTerm = 0.5f * (linearLayerCA + linearDiscCA + linearDriverCA);

    // Sum 0.5 * Cd*A * r^2 for each part (rotationalDragCoefficient*layerHeight is C * avg distance extending outwards)
    float angularLayerCAr2 = layer.rotationalDragCoefficient * layer.height * layer.radius * layer.radius;
    float angularDiscCAr2 = disc.rotationalDragCoefficient * disc.height * disc.radius * disc.radius;
    float angularDriverCAr2 = driver.rotationalDragCoefficient * driver.height * driver.radius * driver.radius;
    angularDragTerm = 0.5f * (angularLayerCAr2 + angularDiscCAr2 + angularDriverCAr2);

    modified = false;  // 2024-12-03
}

BeybladeBody::BeybladeBody() : BeybladeBody(Layer(), Disc(), Driver()) {}

// NEWMESH: Added mesh argument.
BeybladeBody::BeybladeBody(BeybladeMesh* mesh, Layer _layer, Disc _disc, Driver _driver) :
    layer(_layer),
    disc(_disc),
    driver(_driver)
{    
    // NEWUI Copy default layer, etc., and most initialzation below -- too many initializers are gross.
    // The saved structures are useful for the customization UI.
    //
    // NEWUI: TODO.  There are redundant COF and COR in the layer and driver.

    acceleration = glm::vec3(0.0f);
    // NEWUI coefficientOfFriction = 0.2; Now taken from the driver structure.
    // NEWUI coefficientOfRestitution = 0.8; Now taken from the layer structure.
    mass = layer.mass + disc.mass + driver.mass;
    momentOfInertia = layer.momentOfInertia + disc.momentOfInertia + driver.momentOfInertia;
    velocity = glm::vec3(0.0f);


    // NEWMESH:  Use radii and heights from the mesh.  TODO: Remove radii and heights from Layer, etc.

    // Sum 0.5 * Cd * A for each part
    // Use 0.9 for now using cylindrical approximation

    float linearLayerCA = 0.9f * 2 * mesh->heightLayer * mesh->radiusLayer;
    float linearDiscCA = 0.9f * 2 * mesh->heightDisc * mesh->radiusDisc;
    float linearDriverCA = 0.9f * mesh->heightDriver * mesh->heightDriver;
    linearDragTerm = 0.5f*(linearLayerCA + linearDiscCA + linearDriverCA);

    // Sum 0.5 * Cd*A * r^2 for each part (rotationalDragCoefficient*layerHeight is C * avg distance extending outwards)
    float angularLayerCAr2 = layer.rotationalDragCoefficient * mesh->heightLayer * mesh->radiusLayer * mesh->radiusLayer;
    float angularDiscCAr2 = disc.rotationalDragCoefficient * mesh->heightDisc * mesh->radiusDisc * mesh->radiusDisc;
    float angularDriverCAr2 = driver.rotationalDragCoefficient * mesh->heightDriver * mesh->radiusDriver * mesh->radiusDriver;
    angularDragTerm = 0.5f*(angularLayerCAr2 + angularDiscCAr2 + angularDriverCAr2);

    modified = false;  // 2024-12-03
}

//NEWMESH: NO BeybladeBody::BeybladeBody() : BeybladeBody(Layer(), Disc(), Driver()) {}

/*--------------------------------------------Specialized Getters--------------------------------------------*/
glm::vec3 BeybladeBody::getNormal() const {
    glm::vec3 normalizedAngularVelocity = glm::normalize(angularVelocity);
    // If the y-component is negative, reverse the vector
    if (normalizedAngularVelocity.y < 0) {
        normalizedAngularVelocity = -normalizedAngularVelocity;
    }
    return normalizedAngularVelocity;
}

glm::vec3 BeybladeBody::getBottomPosition() const {
    glm::vec3 unitDown = angularVelocity.y < 0 ? glm::normalize(angularVelocity) : -glm::normalize(angularVelocity);
    glm::vec3 tiltedDisplacement = dv3(disc.height + driver.height) * unitDown;
    glm::vec3 bottomPosition = baseCenter + tiltedDisplacement;
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

    glm::vec3 zeroes{ 0 };
    accumulatedAcceleration = zeroes;
    accumulatedAngularAcceleration = zeroes;
    accumulatedVelocity = zeroes;
}

void BeybladeBody::setInitialLaunch(glm::vec3 initialCenter, glm::vec3 initialVelocity, glm::vec3 initialAngularVelocity)
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
    glm::vec3 mx = baseCenter + glm::vec3(layer.radius, layer.height, layer.radius);
    glm::vec3 mn = baseCenter - glm::vec3(layer.radius, disc.height + driver.height, layer.radius);
    return BoundingBox(mx, mn);
}

/*--------------------------------------------Collision Calculations--------------------------------------------*/

float BeybladeBody::sampleRecoil()
{
    return layer.recoilDistribution.sample();
}

std::optional<float> BeybladeBody::distanceOverlap(BeybladeBody* a, BeybladeBody* b) {
    if (!a || !b) {
        throw std::invalid_argument("Null pointer created in Beyblade::inContact");
    }

    glm::vec3 aCenter = a->getCenter();
    glm::vec3 bCenter = b->getCenter();

    BeybladeBody* lowerBey = aCenter.y < bCenter.y ? a : b;
    BeybladeBody* higherBey = lowerBey == a ? b : a;

    // Return nothing (no contact) early if layers do not vertically overlap
    if (lowerBey->getCenter().y + lowerBey->getLayerHeight() < higherBey->getCenter().y) {
        return std::nullopt;
    }

    float diffX = aCenter.x - bCenter.x;
    float diffZ = aCenter.z - bCenter.z;
    float squaredDistance = diffX * diffX + diffZ * diffZ;
    float radiiSum = a->getLayerRadius() + b->getLayerRadius();

    float overlapDistance = radiiSum * radiiSum - squaredDistance;

    // Checks for horizontal overlap based on xz coordinates with radii
    if (overlapDistance > 0) {
        return std::optional<float>(sqrt(overlapDistance));
    }
    return std::nullopt;
}

/*--------------------------------------------Accumulators--------------------------------------------*/

void BeybladeBody::accumulateVelocity(glm::vec3 addedVelocity)
{
    accumulatedVelocity += addedVelocity;
}

void BeybladeBody::accumulateAngularVelocity(glm::vec3 addedAngularVelocity)
{
    accumulatedAngularVelocity += addedAngularVelocity;
}

void BeybladeBody::accumulateAcceleration(glm::vec3 addedAcceleration)
{
    accumulatedAcceleration += addedAcceleration;
}

void BeybladeBody::accumulateAngularAcceleration(glm::vec3 addedAngularAcceleration)
{
    accumulatedAngularAcceleration += addedAngularAcceleration;
}


/*--------------------------------------------Updators--------------------------------------------*/

// Increases or decreases angular speed given angular impulse magnitude
void BeybladeBody::accumulateAngularImpulseMagnitude(float magnitude)
{
    // Spin should never be 0 so no need to check
    glm::vec3 deltaAngularImpulse = dv3(magnitude) * glm::normalize(angularVelocity);
    glm::vec3 deltaAngularVelocity = deltaAngularImpulse / dv3(momentOfInertia);
    accumulateAngularVelocity(deltaAngularVelocity);
}

// Increases or decreases linear speed given linear impulse magnitude
void BeybladeBody::accumulateImpulseMagnitude(float magnitude)
{
    if (glm::length(velocity) < std::numeric_limits<float>::epsilon()) {
        accumulateVelocity(glm::vec3(magnitude, 0.0, 0.0));
        std::cout << "Velocity was 0, used default x ins accumulateImpulse\n";
        return;
    }
    glm::vec3 deltaImpulse = dv3(magnitude) * glm::normalize(velocity);
    glm::vec3 deltaVelocity = deltaImpulse / dv3(mass);
    accumulateVelocity(deltaVelocity);
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

    velocity += accumulatedAcceleration * dv3(deltaTime);
    angularVelocity += accumulatedAngularAcceleration * dv3(deltaTime);

    accumulatedVelocity = accumulatedAngularVelocity = accumulatedAcceleration = accumulatedAngularAcceleration = glm::vec3(0.0);
}

void BeybladeBody::update(float deltaTime)
{
    baseCenter += velocity * dv3(deltaTime);
#if 0
    std::cout << "Base center: " << baseCenter.x << ", " << baseCenter.y << ", " << baseCenter.z << std::endl;
#endif
}