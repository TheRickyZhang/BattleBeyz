////////////////////////////////////////////////////////////////////////////////
// RidigBody.cpp -- Rigid Body object for physics handling -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "RigidBody.h"

#include <iostream>

#include "Buffers.h"
#include "ObjectShader.h"
#include "Utils.h"


RigidBody::RigidBody(const  std::string& _name, const glm::vec3& pos, const glm::vec3& sz, float mass, std::vector<BoundingBox*> bboxes)
        : name(_name), position(pos), mass(mass), velocity(0.0f), acceleration(0.0f),
          angularVelocity(0.0f), orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
          boundingBoxes(bboxes), VAO(0), VBO(0), EBO(0) {
    float x2 = sz.x * sz.x;
    float y2 = sz.y * sz.y;
    float z2 = sz.z * sz.z;
    float factor = mass / 12.0f;
    inertiaTensor = glm::mat3(
            factor * (y2 + z2), 0.0f, 0.0f,
            0.0f, factor * (x2 + z2), 0.0f,
            0.0f, 0.0f, factor * (x2 + y2)
    );
    inverseInertiaTensor = glm::inverse(inertiaTensor);
    aggregateBoundingBox = BoundingBox(glm::vec3(-0.1f), glm::vec3(0.1f));

    setupRigidBodyBuffers();

    updateBoundingBoxes(); // Initial update of bounding boxes
}

RigidBody::~RigidBody() {
    // Unique pointers automatically manage memory, so no need for manual deletion
}


void RigidBody::updateInertiaTensor() {
    glm::mat3 rotationMatrix = glm::mat3_cast(orientation);
    inverseInertiaTensor = rotationMatrix * glm::inverse(inertiaTensor) * glm::transpose(rotationMatrix);
}

void RigidBody::updateBoundingBoxes() {
    // Update each bounding box based on the new position and orientation
    for (auto box : boundingBoxes) {
        box->update(position, orientation);
//        aggregateBoundingBox.expandToInclude(*box);
    }
}


void RigidBody::update(float deltaTime) {
#if 0
    glm::vec3 angularAcceleration = inverseInertiaTensor * torque;
    angularVelocity += angularAcceleration * deltaTime;
    orientation += glm::quat(0.0f, angularVelocity * deltaTime) * orientation * 0.5f;
    orientation = glm::normalize(orientation);
    torque = glm::vec3(0.0f);  // Reset torque
    updateInertiaTensor();
#endif
    updateBoundingBoxes();
}


void RigidBody::setupRigidBodyBuffers() {
    // Vertex coordinates for the bounding box based on min and max points
    glm::vec3 &min = aggregateBoundingBox.min;
    glm::vec3 &max = aggregateBoundingBox.max;
    float vertices[] = {
            // Positions        // Normals         // Texture Coords  // Colors (RGB)
            min.x, min.y, min.z, 0.0, 0.0, -1.0,    0.0, 0.0,         1.0, 0.0, 0.0, // Min corner, red
            max.x, min.y, min.z, 1.0, 0.0, 0.0,     1.0, 0.0,         0.0, 1.0, 0.0, // Max x, green
            max.x, max.y, min.z, 0.0, 1.0, 0.0,     1.0, 1.0,         0.0, 0.0, 1.0, // Max x,y, blue
            min.x, max.y, min.z, -1.0, 0.0, 0.0,    0.0, 1.0,         1.0, 1.0, 0.0, // Max y, yellow
            min.x, min.y, max.z, 0.0, -1.0, 0.0,    0.0, 0.0,         1.0, 0.0, 1.0, // Max z, magenta
            max.x, min.y, max.z, 1.0, 0.0, 0.0,     1.0, 0.0,         0.0, 1.0, 1.0, // Max x,z, cyan
            max.x, max.y, max.z, 0.0, 1.0, 0.0,     1.0, 1.0,         1.0, 0.5, 0.0, // Max x,y,z, orange
            min.x, max.y, max.z, -1.0, 0.0, 0.0,    0.0, 1.0,         0.5, 0.5, 0.5  // Max y,z, grey
    };

    unsigned int indices[] = {
            0, 1, 1, 2, 2, 3, 3, 0,  // Bottom face
            4, 5, 5, 6, 6, 7, 7, 4,  // Top face
            0, 4, 1, 5, 2, 6, 3, 7   // Connecting edges
    };

    setupBuffers(VAO, VBO, EBO, vertices, sizeof(vertices), indices, sizeof(indices), {3, 3, 2, 3});
}


void RigidBody::renderDebug(ObjectShader &shader, const glm::vec3 &viewPos) {
    glm::vec3 &min = aggregateBoundingBox.min;
    glm::vec3 &max = aggregateBoundingBox.max;
    shader.use();
    shader.setObjectRenderParams(glm::mat4(1.0f), glm::vec3(1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr); // 24 is the number of indices for 12 lines
    glBindVertexArray(0);

    GL_CHECK("RigidBody");
}
