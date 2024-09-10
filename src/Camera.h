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
    explicit Camera(const glm::vec3& position, float yaw = -90.0f, float pitch = 0.0f, float roll = 0.0f, PhysicsWorld* world = nullptr);

    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up{};
    glm::vec3 Right{};
    glm::vec3 WorldUp;

    // Euler Angles (add roll)
    float Yaw;
    float Pitch;
    float Roll;

    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

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
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
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