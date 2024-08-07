#pragma once

#include "GameObject.h"
#include "Texture.h"
#include "Utils.h"
#include "Buffers.h"
#include "BoundingBox.h"
#include "PhysicsWorld.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <memory>


class Stadium : public GameObject {
public:
    Stadium(unsigned int vao, unsigned int vbo, unsigned int ebo, const glm::vec3& pos, const glm::vec3& col,
            const glm::vec3& ringColor, const glm::vec3& crossColor, float radius, float curvature, int numRings,
            int verticesPerRing, Texture* texture, float textureScale, PhysicsWorld* physicsWorld);

    void update() {}
    void initializeMesh() override;
    void render(ShaderProgram &shader, const glm::vec3 &lightColor, const glm::vec3 &lightPos) override;

    ImmovableRigidBody* body;
protected:
    void generateMeshData();

private:
    Texture* texture;
    PhysicsWorld* physicsWorld;
    float radius;
    float curvature;
    int verticesPerRing;
    int numRings;
    glm::vec3 ringColor;
    glm::vec3 crossColor;

    std::vector<std::unique_ptr<BoundingBox>> triangleBoundingBoxes;

    float textureScale = 1.0f;
};