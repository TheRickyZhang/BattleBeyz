////////////////////////////////////////////////////////////////////////////////
// Camera.h -- Camera handling include -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "BoundingBox.h"
#include "RigidBody.h"
#include "PhysicsWorld.h"

class Camera {
public:
    // Default constructor
    Camera() : position(glm::vec3(0.0f, 0.0f, 0.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)), right(glm::vec3(1.0f, 0.0f, 0.0f)), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), yaw(-90.0f), pitch(0.0f), roll(0.0f), movementSpeed(2.5f), mouseSensitivity(0.1f), zoom(45.0f), physicsWorld(nullptr) {
        updateCameraVectors();
    }
    explicit Camera(const glm::vec3& position, float yaw = -90.0f, float pitch = 0.0f, float roll = 0.0f, PhysicsWorld* world = nullptr);

    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up{};
    glm::vec3 right{};
    glm::vec3 worldUp;

    // Euler Angles (add roll)
    float yaw;
    float pitch;
    float roll;

    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    // TODO: Add different camera states of (free, fixedFocus, followBeyblade, circularPath)
    // These all have their own data needed to funciton, which needs to be figured out

    // Camera rigid body
    // TODO: Delete, outdated. There should no longer be any rigidBodies, only beybladeBodies and stadiumBodies.
    // Free-moving camera restricted by box coordinates is fine for now
    RigidBody* body;
    PhysicsWorld* physicsWorld;

    // Camera matrices
    [[nodiscard]] glm::mat4 getViewMatrix() const;
    void processKeyboard(int direction, float deltaTime, bool boundCamera);
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainpitch = true);
    void processMouseScroll(float yoffset);

private:
    void updateCameraVectors();
    void applyBoundaries(glm::vec3 &position) const;
};

struct CameraState {
    Camera* camera;
    double lastX;
    double lastY;
    bool firstMouse;

    CameraState(Camera* cam, double startX, double startY)
            : camera(cam), lastX(startX), lastY(startY), firstMouse(true) {}
};