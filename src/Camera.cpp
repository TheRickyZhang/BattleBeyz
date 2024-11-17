////////////////////////////////////////////////////////////////////////////////
// Camera.cpp -- Camera handling -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#include "Camera.h"

/**
* Constructor
*/

Camera::Camera(const glm::vec3& position, float yaw, float pitch, float roll, PhysicsWorld* world, BoundingBox box) :
    position(position),yaw(yaw), pitch(pitch), roll(roll),
    movementSpeed(1.0f), mouseSensitivity(0.1f), zoom(45.0f), physicsWorld(world), movementRange(box) {
    updateCameraVectors();

    // Initialize the camera's rigid body
    std::vector<BoundingBox*> bboxes;
    bboxes.push_back(new BoundingBox(glm::vec3(position - glm::vec3(1.0f)), glm::vec3(position + glm::vec3(1.0f))));
    body = new RigidBody("camera", position, glm::vec3(0.02f), 0.79f, std::move(bboxes));
}

/**
* Get current view matrix.
*
* @return the current transformation matrix.
*/

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

/**
* Returns new position after applying any potential colisions
* TOLOOK: Maybe have a small buffer around camera position to reduce clipping
*/

glm::vec3 Camera::applyCollisions(const glm::vec3& currPos, glm::vec3& nextPos) const {
    for (const Beyblade* beyblade : physicsWorld->getBeyblades()) {
        BoundingBox beybladeBoundary = beyblade->getRigidBody()->getBoundingBox();
        BoundingBox cameraBoundary(nextPos - glm::vec3(0.5f), nextPos + glm::vec3(0.5f));
        if (BoundingBox::intersect(cameraBoundary, beybladeBoundary)) {
            nextPos = beybladeBoundary.closestPointOutside(nextPos);
        }
    }

    for (const Stadium* stadium : physicsWorld->getStadiums()) {
        StadiumBody* body = stadium->getRigidBody();
        if (body->isInside(nextPos.x, nextPos.z)) {
            float surfaceY = body->getY(nextPos.x, nextPos.z);
            if (nextPos.y < surfaceY) {
                nextPos.y = surfaceY;
            }
        }
    }
    return nextPos;
}


/**
* Process keyboard input at affects the view.
*
* @param action                     [in] An Action value.
*
* @param deltaTime                  [in] Time in seconds since last event.
*
* boundCamera                       [in] True if camera is bound, meaning it is immobile.
* 
* Use WASD, QE for camera movement
*/

void Camera::processKeyboard(Action action, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    glm::vec3 newPosition = position;

    bool isMoving = false;

    glm::vec3 direction = getDirection(action);
    if (direction != glm::vec3(0.0f)) {
        newPosition += direction * velocity;
        isMoving = true;
    }

     // If no movement occurred, set the velocity to zero
    if (!isMoving) {
        body->velocity = glm::vec3(0.0f);
        return;
    }

    // This works
    newPosition = movementRange.closestPointInside(newPosition);

    // TODO: Camera gets stuck on a beyblade or stadium when you get too close. Probably becuase no buffer / incorrect logic
    if (hasCollision && physicsWorld != nullptr) {
        newPosition = applyCollisions(position, newPosition);
    }
    position = newPosition;

    std::cout << "Camera position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
    //std::cout << "Body position: " << body->position.x << ", " << body->position.y << ", " << body->position.z << std::endl;
}

/**
* Process mouse movement. Pitch is always constrained to prevent gimbal lock.
* 
* @param xoffset                    [in] Mouse X position relative to previous position.
* 
* @param yoffset                    [in] Mouse Y position relative to previous position.
* 
* @param constrainpitch             [in] True if pitch needs to be constrained.
*/

void Camera::processMouseMovement(float xoffset, float yoffset ) {
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
* @param yoffset                    [in] Y Offset.  TODO: What is this?
*/

void Camera::processMouseScroll(float yoffset) {
    movementSpeed += yoffset * 0.5f;
    if (movementSpeed < 0.1f) {
        movementSpeed = 0.1f;
    }
    if (movementSpeed > 10.0f) {
        movementSpeed = 10.0f;
    }
}

/**
* update camera vectors.
*
* Computes front, up, right based on yaw and pitch. Applies roll.
*/

void Camera::updateCameraVectors() {
    // Calculate the new front vector based on yaw and pitch
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    // Recalculate the right and up vectors without roll
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));

    // Apply roll around the front vector
    if (roll != 0.0f) {
        glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll), front);
        right = glm::normalize(glm::vec3(rollMatrix * glm::vec4(right, 0.0f)));
        up = glm::normalize(glm::vec3(rollMatrix * glm::vec4(up, 0.0f)));
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

glm::vec3 Camera::getDirection(Action action) const {
    switch (action) {
        case Action::MoveForward: return front;
        case Action::MoveBackward: return -front;
        case Action::MoveLeft: return -right;
        case Action::MoveRight: return right;
        case Action::MoveDown: return -up;
        case Action::MoveUp: return up;
    }
    return glm::vec3(0.0f);
}