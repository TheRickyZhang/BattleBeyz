////////////////////////////////////////////////////////////////////////////////
// Camera.cpp -- Camera handling -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#include "Camera.h"

/**
* Constructor
*/

Camera::Camera(const glm::vec3& position, float yaw, float pitch, float roll, PhysicsWorld* world) :
    Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(10.0f), MouseSensitivity(0.1f), Zoom(45.0f),
    Position(position), WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)), Yaw(yaw), Pitch(pitch), Roll(roll),
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
    return glm::lookAt(Position, Position + Front, Up);
}

/**
* TODO: Apply boundaries for "restricted" camera mode, where the camera cannot leave a certain area
*/

void Camera::applyBoundaries(glm::vec3& position) const {
    if (!physicsWorld) return;
    //// Temporarily move the body's position to check for collisions
    //glm::vec3 originalPosition = body->position;
    //body->position = position;

    //std::cout << "Applying boundaries. New position: "
    //    << position.x << ", " << position.y << ", " << position.z << "\n";

    //for (const auto beybladeBody : physicsWorld->getBeybladeBodies()) {
    //    for (const auto box : beybladeBody->boundingBoxes) {

    //    }
    //}

    //body->position = originalPosition; // Restore original position
}

/**
* Process keyboard input at affects the view.
*
* @param direction                  [in] A GLFW_KEY_value.
*
* @param deltaTime                  [in] Time in seconds since last event.
*
* boundCamera                       [in] True if camera is bound, meaning it is immobile.
* 
* Use WASD, QE for camera movement
*/

void Camera::processKeyboard(int direction, float deltaTime, bool boundCamera) {
    float velocity = MovementSpeed * deltaTime;
    glm::vec3 newPosition = Position;

    bool isMoving = false;

    switch (direction) {
        case GLFW_KEY_W:
            newPosition += Front * velocity;
            isMoving = true;
            break;
        case GLFW_KEY_S:
            newPosition -= Front * velocity;
            isMoving = true;
            break;
        case GLFW_KEY_A:
            newPosition -= Right * velocity;
            isMoving = true;
            break;
        case GLFW_KEY_D:
            newPosition += Right * velocity;
            isMoving = true;
            break;
        case GLFW_KEY_Q:
            newPosition -= Up * velocity;
            isMoving = true;
            break;
        case GLFW_KEY_E:
            newPosition += Up * velocity;
            isMoving = true;
            break;
        case GLFW_KEY_ESCAPE:
            Zoom = 1.0f;
            break;
    }

    if (!isMoving) {
        body->velocity = glm::vec3(0.0f);
    }

    if (boundCamera) {
        applyBoundaries(newPosition);
    }

    Position = newPosition;
    //std::cout << "Camera position: " << Position.x << ", " << Position.y << ", " << Position.z << std::endl;
    // Update the body's position to match the camera
    body->position = Position;
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
* @param constrainPitch             [in] True if pitch needs to be constrained.
*/

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f) {
            Pitch = 89.0f;
        }
        if (Pitch < -89.0f) {
            Pitch = -89.0f;
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
    MovementSpeed += yoffset * 0.5f;
    if (MovementSpeed < 0.1f) {
        MovementSpeed = 0.1f;
    }
    if (MovementSpeed > 10.0f) {
        MovementSpeed = 10.0f;
    }
}

/**
* Update camera vectors.
*
* TODO: More comments.
*/

void Camera::updateCameraVectors() {
    // Calculate the new Front vector
    float x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    float y = sin(glm::radians(Pitch));
    float z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    glm::vec3 front = glm::vec3(x, y, z);
    Front = glm::normalize(front);

    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0
    Up = glm::normalize(glm::cross(Right, Front));       // the more you look up or down which results in slower movement.

    // Apply roll
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(Roll), Front);
    Right = glm::vec3(rot * glm::vec4(Right, 0.0f));
    Up = glm::vec3(rot * glm::vec4(Up, 0.0f));
}
