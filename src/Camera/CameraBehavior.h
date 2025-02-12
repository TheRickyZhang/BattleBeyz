#pragma once

#include <memory>

class Camera;

// Strategy Pattern - seems to fit best here
// Use a CameraBehavior* in Camera, but require Camera& instance in the constructor / update function
class CameraBehavior {
public:
    virtual void update(Camera& camera, float deltaTime) = 0;
    virtual ~CameraBehavior() {}
};

