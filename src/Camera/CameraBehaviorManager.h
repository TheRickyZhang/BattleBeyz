#pragma once

#include <memory>
#include "Camera.h"

class CameraBehavior {
public:
    virtual void update(Camera& camera, float deltaTime) = 0;
    virtual ~CameraBehavior() {}
};

class HoldBeybladeBehavior : public CameraBehavior {
public:
    HoldBeybladeBehavior(const glm::vec3& center, float distance)
        : beybladeCenter(center), distance(distance) {
    }

    void update(Camera& camera, float deltaTime) override {
        glm::vec3 targetPosition = beybladeCenter + glm::vec3(0.0f, 5.0f, -distance);
        camera.position = glm::mix(camera.position, targetPosition, 0.1f);
    }

private:
    glm::vec3 beybladeCenter;
    float distance;
};

