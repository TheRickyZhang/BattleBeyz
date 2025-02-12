#include "PanningBehavior.h"
#include "Camera.h"

void PanningBehavior::update(Camera& camera, float deltaTime) {
    // Update the angle and position
    currentAngle += angularVelocity * deltaTime;
    currentAngle = glm::mod(currentAngle, 2.0f * glm::pi<float>());

    glm::vec3 center = stadium->getCenter().value();
    glm::vec3 nextPos = center + glm::vec3(radius * cos(currentAngle), 10.0f, radius * sin(currentAngle));
    camera.setPositionSmooth(nextPos);
    camera.focus(center);
    //camera.enforceBoundaries();
}