////////////////////////////////////////////////////////////////////////////////
// RidigBody.cpp -- Rigid Body object for physics handling -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "RigidBody.h"

RigidBody::RigidBody(const  std::string& _name, const glm::vec3& pos, const glm::vec3& sz, float mass, std::vector<BoundingBox*> bboxes)
        : name(_name), position(pos), mass(mass), velocity(0.0f), acceleration(0.0f), force(0.0f),
          angularVelocity(0.0f), torque(0.0f), orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
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

    setupBuffers();

    updateBoundingBoxes(); // Initial update of bounding boxes
}

RigidBody::~RigidBody() {
    // Unique pointers automatically manage memory, so no need for manual deletion
}

void RigidBody::applyForce(const glm::vec3& f) {
    force += f;
}

void RigidBody::applyTorque(const glm::vec3& t) {
    torque += t;
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
    // Linear dynamics
    acceleration = force / mass;
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
    force = glm::vec3(0.0f);  // Reset force

    // Angular dynamics

#if 0
    glm::vec3 angularAcceleration = inverseInertiaTensor * torque;
    angularVelocity += angularAcceleration * deltaTime;
    orientation += glm::quat(0.0f, angularVelocity * deltaTime) * orientation * 0.5f;
    orientation = glm::normalize(orientation);
    torque = glm::vec3(0.0f);  // Reset torque
    updateInertiaTensor();
#endif

    // Update bounding boxes

    updateBoundingBoxes();
}


void RigidBody::setupBuffers() {
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

    std::cout << "Generated VAO: " << VAO << std::endl;
    ::setupBuffers(VAO, VBO, EBO, vertices, sizeof(vertices), indices, sizeof(indices));
    std::cout << "Generated VAO: " << VAO << std::endl;
}


void RigidBody::renderDebug(ShaderProgram &shader, const glm::vec3 &viewPos) {
    glm::vec3 &min = aggregateBoundingBox.min;
    glm::vec3 &max = aggregateBoundingBox.max;
    shader.use();

    // Set shader uniforms
    shader.setUniformMat4("model", glm::mat4(1.0f));
    shader.setUniformVec3("view", viewPos);
    
    // Check if VAO is valid
    std::cout << "VAO: " << VAO << std::endl;

    // Check for any previous OpenGL errors
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error before glBindVertexArray: " << error << std::endl;
    }

    glm::vec3 color(1.0f, 1.0f, 1.0f);
    shader.setObjectColor(&color);

    GL_CHECK(glBindVertexArray(VAO));
    GL_CHECK(glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr)); // 24 is the number of indices for 12 lines
    GL_CHECK(glBindVertexArray(0));

    shader.setObjectColor(nullptr);
}
