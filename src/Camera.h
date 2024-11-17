////////////////////////////////////////////////////////////////////////////////
// Camera.h -- Camera handling include -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>
#include "BoundingBox.h"
#include "RigidBody.h"
#include "PhysicsWorld.h"
#include "InputManager.h"

class Camera {
public:
    // Default constructor
    Camera() : position(glm::vec3(0.0f, 0.0f, 0.0f)), yaw(-90.0f), pitch(0.0f), roll(0.0f), physicsWorld(nullptr),
               movementSpeed(1.0f), mouseSensitivity(0.1f), zoom(45.0f),
                movementRange(BoundingBox(glm::vec3(-1e6), glm::vec3(1e6))) {
        updateCameraVectors();
    }
    // Specific constructor - use this
    Camera(const glm::vec3& position, float yaw = -90.0f, float pitch = 0.0f, float roll = 0.0f, PhysicsWorld* world = nullptr,
        BoundingBox box = BoundingBox(glm::vec3(-1e6), glm::vec3(1e6)));

    glm::vec3 position;
    const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f); // TOLOOK: Only reason this would change is differnt gravity options?

    // Can always be inferred from updateCameraVectors() after worldUp and pitch/yaw are set
    glm::vec3 front{};
    glm::vec3 up{};
    glm::vec3 right{};

    // Euler Angles
    float yaw;               // [-180, 180]  (left, right)
    float pitch;             // [-89, 89]    (up, down)
    float roll;              // [-180, 180]  (twisting)

    // Camera options
    float movementSpeed;     // [0.1, 10]
    float mouseSensitivity;  // [0.02, 0.5]
    float zoom;              // [0.1, 10]

    BoundingBox movementRange;
    bool hasCollision = true;
    bool boundCamera = true;


    // TODO: Add different camera states of (free, fixedFocus, followBeyblade, circularPath)
    // These all have their own data needed to funciton, which needs to be figured out

    // Camera rigid body
    // TODO: Delete, outdated. There should no longer be any rigidBodies, only beybladeBodies and stadiumBodies.
    // Free-moving camera restricted by box coordinates is fine for now
    RigidBody* body;
    PhysicsWorld* physicsWorld;

    // Camera matrices
    [[nodiscard]] glm::mat4 getViewMatrix() const;
    void processKeyboard(Action action, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset);
    void processMouseScroll(float yoffset);

private:
    glm::vec3 getDirection(Action action) const;
    void updateCameraVectors();
    glm::vec3 applyCollisions(const glm::vec3& currPos, glm::vec3& nextPos) const;
    void constrainAngles();
};

struct CameraState {
    Camera* camera;
    double lastX;
    double lastY;
    bool firstMouse;

    CameraState(Camera* cam, double startX, double startY)
            : camera(cam), lastX(startX), lastY(startY), firstMouse(true) {}
};