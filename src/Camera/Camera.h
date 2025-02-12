////////////////////////////////////////////////////////////////////////////////
// Camera.h -- Camera handling include -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include "BoundingBox.h"
#include "CameraBehavior.h"

enum Action;
class PhysicsWorld;
class BeybladeBody;
class Stadium;
class InputManager;

/*
* Free: Unrestricted movement and view.
* Attached: Movement bound to specific beyblade, unrestricted view
* Panning: Movement and view bound to a specific focal point, rotates around the stadium.
*/
enum struct CameraMode {
    FREE,
    ATTACHED,
    PANNING
};

// STANDARDS: Better encapsulation
class Camera {
public:
    // TODO: Require boundingbox, move to .cpp
    Camera(const glm::vec3& position, const glm::vec3& viewPoint, PhysicsWorld* world, float x, float y, BoundingBox box = BoundingBox(glm::vec3(-1e6), glm::vec3(1e6)));

    // TODO: Manage these better with function objects / strategy patterns (composition of inheritance)
    void setBehavior(std::unique_ptr<CameraBehavior> behavior);
    void clearBehavior();

    // Must call these before changing the mode (initial only works in free UNTIL attached to beybalde/stadium)
    void setFollowingBey(BeybladeBody* bey) { followingBey = bey; }
    void setPanningVariables(Stadium* stadium);
    void changeCameraMode(CameraMode newMode) { activeMode = newMode; }

    static const glm::vec3 worldUp;
    glm::vec3 position;

    // Euler Angles
    float yaw = -90.0f;             // [-180, 180]  (left, right)
    float pitch = 0.0f;             // [-89, 89]    (up, down)
    float roll = 0.0f;              // [-180, 180]  (twisting)

    // Can always be inferred from updateCameraVectors() and angles
    glm::vec3 front{};
    glm::vec3 up{};
    glm::vec3 right{};

    // Camera options
    float movementSpeed = 1.0f;     // [0.1, 10]
    float mouseSensitivity = 0.1f;  // [0.02, 0.5]
    float zoom = 45.0f;             // [0.1, 10]

    BoundingBox movementRange;
    bool hasCollision = true;

    // For screen input with camera movements
    float lastX;
    float lastY;
    bool firstMouse;

    CameraMode activeMode = CameraMode::FREE;
    PhysicsWorld* physicsWorld;

    void update(float deltaTime);
    void focus(const glm::vec3& viewPoint, bool teleport = false);
    void setPositionSmooth(const glm::vec3& targetPosition, float alpha = 0.1f);

    // Camera matrices
    glm::mat4 getViewMatrix() const;
    void processKeyboard(Action action, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset);
    void processMouseScroll(float scrollAmount);

    // Callbacks for input
    void handleMouseDrag(InputManager& im);
    void handleMouseScroll(InputManager& im);

    // Inline setters (Specific setters commented for now to focus on core fucnitonality)
    //void setHeightAbove(const glm::vec3& height) { heightAbove = height; }
    //void setRadius(const glm::vec3& r) { radius = r; }
    //void setViewCenter(const glm::vec3& center) { viewCenter = center; }
    //void setRotationCenter(const glm::vec3& center) { rotationCenter = center; }
    //void setCurrentAngle(float angle) { currentAngle = angle; }
    //void setAngularVelocity(float velocity) { angularVelocity = velocity; }s
private:
    glm::vec3 getDirection(Action action) const;
    void updateCameraVectors();
    glm::vec3 applyCollisions(const glm::vec3& currPos, glm::vec3& nextPos) const;
    void constrainAngles();

    // Information needed for all the camera modes - looked into alternatives and I think this is the best way to do switching optimally
    
    float heightAbove = 0.25f;

    // For behaviors
    std::unique_ptr<CameraBehavior> behavior;

    // For attached
    BeybladeBody* followingBey{};

    // For panning
    float radius{};
    glm::vec3 viewCenter{};
    glm::vec3 rotationCenter{};
    float currentAngle{};    // Radians
    float angularVelocity{}; // Positive = CCW, Negative = CW;
};
