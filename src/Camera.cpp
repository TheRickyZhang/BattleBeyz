////////////////////////////////////////////////////////////////////////////////
// Camera.cpp -- Camera handling -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#include "Camera.h"

/**
* Constructor
*/

Camera::Camera(const glm::vec3& position, float yaw, float pitch, float roll, PhysicsWorld* world) :
    front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(10.0f), mouseSensitivity(0.1f), zoom(45.0f),
    position(position), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), yaw(yaw), pitch(pitch), roll(roll),
    physicsWorld(world) {
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
* TODO: Apply boundaries for "restricted" camera mode, where the camera cannot leave a certain area
*/

void Camera::applyBoundaries(glm::vec3& position) const {
    if (!physicsWorld) return;
    //// Temporarily move the body's position to check for collisions
    //glm::vec3 originalposition = body->position;
    //body->position = position;

    //std::cout << "Applying boundaries. New position: "
    //    << position.x << ", " << position.y << ", " << position.z << "\n";

    //for (const auto beybladeBody : physicsWorld->getBeybladeBodies()) {
    //    for (const auto box : beybladeBody->boundingBoxes) {

    //    }
    //}

    //body->position = originalposition; // Restore original position
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

void Camera::processKeyboard(Action action, float deltaTime, bool boundCamera) {
    float velocity = movementSpeed * deltaTime;
    glm::vec3 newPosition = position;

    bool isMoving = false;

    switch (action) {
    case Action::MoveForward:
            newPosition += front * velocity;
            isMoving = true;
            break;
    case Action::MoveBackward:
            newPosition -= front * velocity;
            isMoving = true;
            break;
    case Action::MoveLeft:
            newPosition -= right * velocity;
            isMoving = true;
            break;
    case Action::MoveRight:
            newPosition += right * velocity;
            isMoving = true;
            break;
    case Action::MoveDown:
            newPosition -= up * velocity;
            isMoving = true;
            break;
    case Action::MoveUp:
            newPosition += up * velocity;
            isMoving = true;
            break;
    }

    if (!isMoving) {
        body->velocity = glm::vec3(0.0f);
    }

    if (boundCamera) {
        applyBoundaries(newPosition);
    }

    position = newPosition;
    //std::cout << "Camera position: " << Position.x << ", " << Position.y << ", " << Position.z << std::endl;
    // update the body's position to match the camera
    body->position = position;
    body->updateBoundingBoxes();
    //std::cout << "Body position: " << body->position.x << ", " << body->position.y << ", " << body->position.z << std::endl;
}

/**
* Process mouse movement.
* 
* @param xoffset                    [in] Mouse X position relative to previous position.
* 
* @param yoffset                    [in] Mouse Y position relative to previous position.
* 
* @param constrainpitch             [in] True if pitch needs to be constrained.
*/

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainpitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainpitch) {
        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }
    }

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
* TODO: More comments.
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