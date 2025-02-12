#include "AttachedBehavior.h"
#include "Camera.h"

void AttachedBehavior::update(Camera& camera, float deltaTime) {
    if (!beyblade) return;
    glm::vec3 targetPos = beyblade->getCenter().value() + glm::vec3(0.0f, heightAbove, 0.0f);
    camera.setPositionSmooth(targetPos, 0.1f);

    // If following "behind" bey, need to continuously focus??
    //camera.focus(targetPos); 
    //camera.enforceBoundaries();
}