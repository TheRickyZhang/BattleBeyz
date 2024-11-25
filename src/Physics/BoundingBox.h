////////////////////////////////////////////////////////////////////////////////
// BoundingBox.h -- Bounding Box object include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/gtx/quaternion.hpp>
#include "ShaderProgram.h"
#include "Buffers.h"

// Currently unused.
class BoundingBox {
public:
    glm::vec3 min;
    glm::vec3 max;

    GLuint VAO{}, VBO{}, EBO{};

    BoundingBox();
    BoundingBox(const glm::vec3& min, const glm::vec3& max);
    ~BoundingBox();

    // These ones are actually used
    static bool intersect(const BoundingBox& a, const BoundingBox& b);
    void renderDebug(ShaderProgram &shader, const glm::vec3& bodyPosition);

    // Naive implementaiton that clamps limits
    glm::vec3 closestPointOutside(const glm::vec3& point) const;
    glm::vec3 closestPointInside(const glm::vec3& point) const;

    // Unused
    [[nodiscard]] bool intersectsSphere(const glm::vec3& center, float radius) const;
    void update(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
    void update(const glm::vec3& position, const glm::quat& orientation);
    void expandToInclude(const BoundingBox& other);
    void expandToInclude(const glm::vec3& point);
private:
    void setupBuffers();
};