////////////////////////////////////////////////////////////////////////////////
// RidigBody.h -- Rigid Body include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <memory>
#include "BoundingBox.h"
#include "Utils.h"

// TODO: Delete, not needed anymore. REALLY?
class RigidBody {
public:
    RigidBody(const std::string& _name, const glm::vec3& pos, const glm::vec3& sz, float mass, std::vector<BoundingBox*> bboxes = {});
    virtual ~RigidBody(); // Destructor to manage memory

    void applyForce(const glm::vec3& f);
    void applyTorque(const glm::vec3& t);

    virtual void update(float deltaTime);

    void setupBuffers();
    void renderDebug(ShaderProgram& shader, const glm::vec3& viewPos);

    void updateBoundingBoxes();

public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    std::vector<BoundingBox*> boundingBoxes;
    float mass;
    glm::vec3 force;
    glm::vec3 angularVelocity;  // Angular velocity vector
    glm::vec3 torque;           // Accumulated torque
    glm::quat orientation;      // Orientation quaternion
    glm::mat3 inertiaTensor;    // Inertia tensor in the body frame
    glm::mat3 inverseInertiaTensor; // Inverse inertia tensor in the world frame
    BoundingBox aggregateBoundingBox; // Aggregate bounding box
    std::string name;           // Useful for debugging

private:
    void updateInertiaTensor();

    GLuint VAO, VBO, EBO;
};

class ImmovableRigidBody : public RigidBody {
public:
    ImmovableRigidBody(const std::string& _name, const glm::vec3& pos, const glm::vec3& sz, std::vector<BoundingBox*> bboxes = {})
            : RigidBody(_name, pos, sz, FLT_MAX, std::move(bboxes)) {} // Infinite mass for immovable body

    void update(float deltaTime) override {
        // No dynamics update for immovable bodies
    }
};
