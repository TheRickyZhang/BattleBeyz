////////////////////////////////////////////////////////////////////////////////
// Camera.cpp -- Camera handling -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#include "Camera.h"
using namespace glm;

const vec3 Camera::worldUp = vec3(0.0f, 1.0f, 0.0f);
/**
* Constructor
*/

Camera::Camera(const vec3& pos, const vec3& viewPoint, PhysicsWorld* world, float x, float y,
    BoundingBox box)
    : position(pos), physicsWorld(world), lastX(x), lastY(y), movementRange(box)
{
    focus(viewPoint);  // Initializes pitch, yaw, roll
    updateCameraVectors();
}

void Camera::setPanningVariables(StadiumBody* stadiumBody) {
    radius = 1.5f * (float)stadiumBody->getRadius();
    viewCenter = stadiumBody->getCenter();
    rotationCenter = viewCenter; rotationCenter.y += stadiumBody->getYLocal(radius) + heightAbove;
    currentAngle = 0;
    angularVelocity = 0.5;
}

/**
* Get current view matrix.
*
* @return the current transformation matrix.
*/

mat4 Camera::getViewMatrix() const {
    return lookAt(position, position + front, up);
}

/**
* Returns new position after applying any potential colisions
* TOLOOK: Maybe have a small buffer around camera position to reduce clipping
*/

vec3 Camera::applyCollisions(const vec3& currPos, vec3& nextPos) const {
    for (Beyblade* beyblade : physicsWorld->getBeyblades()) {
        BoundingBox beybladeBoundary = beyblade->getRigidBody()->getBoundingBox();
        BoundingBox cameraBoundary(nextPos - vec3(0.5f), nextPos + vec3(0.5f));
        if (BoundingBox::intersect(cameraBoundary, beybladeBoundary)) {
            nextPos = beybladeBoundary.closestPointOutside(nextPos);
        }
    }

    for (const Stadium* stadium : physicsWorld->getStadiums()) {
        StadiumBody* body = stadium->getRigidBody();
        if (body->isInside(nextPos.x, nextPos.z)) {
            float surfaceY = (float)body->getY(nextPos.x, nextPos.z);
            if (nextPos.y < surfaceY) {
                nextPos.y = surfaceY;
            }
        }
    }
    return nextPos;
}

/**
* Automatic camera position updates
*/
void Camera::update(float deltaTime) {
    if (activeMode == CameraMode::FREE) return;
    else if (activeMode == CameraMode::ATTACHED) {
        // Update position
        if (followingBey == nullptr) {
            std::cout << "Uninitialized Bey" << std::endl;
        }
        else {
            position = followingBey->getCenter();
            position.y += heightAbove;
        }
    }
    else if (activeMode == CameraMode::PANNING) {
        // Update position
        currentAngle += angularVelocity * deltaTime;
        currentAngle = mod(currentAngle, 2.0f * pi<float>());

        position = rotationCenter;
        position.x += radius * cos(currentAngle);
        position.z += radius * sin(currentAngle);
        focus(viewCenter);
    }
    else {
        std::cout << "HUH" << std::endl;
    }
}

/**
* Manual camera position updates by keyboard. Only applies to Free state
*
* @param action                     [in] An Action value.
*
* @param deltaTime                  [in] Time in seconds since last event.
* 
* Use WASD, QE for camera movement
*/

void Camera::processKeyboard(Action action, float deltaTime) {
    if (activeMode != CameraMode::FREE) {
        return;
    }

    // TOLOOK: Ensure that removal of isMoving flags does not break anything
    vec3 newPosition = position;
    vec3 direction = getDirection(action);
    if (direction != vec3(0.0f)) {
        newPosition += direction * movementSpeed * deltaTime;
    }

    // This works
    newPosition = movementRange.closestPointInside(newPosition);
    // TODO: Camera gets stuck on a beyblade or stadium when you get too close. Probably becuase no buffer / incorrect logic
    //if (hasCollision && physicsWorld != nullptr) {
    //    newPosition = applyCollisions(position, newPosition);
    //}
    position = newPosition;
    //std::cout << "Action clicked" << action << std::endl;
    //std::cout << "Camera position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
}

/**
* Process mouse movement for perspective change. Pitch is always constrained to prevent gimbal lock.
* 
* @param xoffset                    [in] Mouse X position relative to previous position.
* 
* @param yoffset                    [in] Mouse Y position relative to previous position.
* 
*/
void Camera::processMouseMovement(float xoffset, float yoffset ) {
    //if (activeMode == CameraMode::PANNING) {
    //    return;
    //}
    
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    yaw += xoffset;
    pitch += yoffset;

    constrainAngles();
    updateCameraVectors();
}

/**
* Process mouse scroll.
*
* @param scrollAmount              [in] scrollAmount.
*/

void Camera::processMouseScroll(float scrollAmount) {
    movementSpeed += scrollAmount * 0.5f;
    if (movementSpeed < 0.1f) {
        movementSpeed = 0.1f;
    }
    if (movementSpeed > 10.0f) {
        movementSpeed = 10.0f;
    }
}

/**
* Call after every update to yaw/pitch to update front, up, right.
*/
void Camera::updateCameraVectors() {
    // Calculate the new front vector based on yaw and pitch
    front.x = cos(radians(yaw)) * cos(radians(pitch));
    front.y = sin(radians(pitch));
    front.z = sin(radians(yaw)) * cos(radians(pitch));
    front = normalize(front);

    // Recalculate the right and up vectors without roll
    right = normalize(cross(front, worldUp));
    up = normalize(cross(right, front));

    // Apply roll around the front vector
    if (roll != 0.0f) {
        mat4 rollMatrix = rotate(mat4(1.0f), radians(roll), front);
        right = normalize(vec3(rollMatrix * vec4(right, 0.0f)));
        up = normalize(vec3(rollMatrix * vec4(up, 0.0f)));
    }
}

// Yaw and roll overflow, while pitch is constrained (gimbal lock)
void Camera::constrainAngles() {
    if (yaw > 180.0f) {
        yaw -= 360.0f;
    }
    else if (yaw < -180.0f) {
        yaw += 360.0f;
    }
    pitch = std::min(pitch, 89.0f);
    pitch = std::max(pitch, -89.0f);
    if (roll > 180.0f) {
        roll -= 360.0f;
    }
    else if (roll < -180.0f) {
        roll += 360.0f;
    }
}

vec3 Camera::getDirection(Action action) const {
    switch (action) {
        case Action::MoveForward: return front;
        case Action::MoveBackward: return -front;
        case Action::MoveLeft: return -right;
        case Action::MoveRight: return right;
        case Action::MoveDown: return -up;
        case Action::MoveUp: return up;
    }
    return vec3(0.0f);
}

/*
* Makes the camera view a specific point from its current position.
* 
* @param viewPoint [in] location camera should look at
* @param teleport  [in] whether to set the position close to the viewPoint. False by default.
*/
void Camera::focus(const vec3& viewPoint, bool teleport) {
    vec3 direction = normalize(viewPoint - position);
    yaw = degrees(atan2(direction.z, direction.x));
    pitch = degrees(asin(direction.y));
    // Bring the camera close to the object along same axis
    if (teleport) {
        position = viewPoint - vec3(2.0f) * direction;
    }
    updateCameraVectors();
}