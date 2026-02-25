////////////////////////////////////////////////////////////////////////////////
// BoundingBox.h -- Bounding Box object include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

/*
* Axis-aligned bounding box. Pure CPU geometry: holding GL buffers here meant every
* getBoundingBox() call allocated and freed a VAO. Drawing lives in BoundingBoxRenderer.
*/
class BoundingBox {
public:
    glm::vec3 min;
    glm::vec3 max;

    BoundingBox();
    BoundingBox(const glm::vec3& min, const glm::vec3& max);

    static bool intersect(const BoundingBox& a, const BoundingBox& b);

    // Naive implementaiton that clamps limits
    glm::vec3 closestPointOutside(const glm::vec3& point) const;
    glm::vec3 closestPointInside(const glm::vec3& point) const;

    // Unused
    [[nodiscard]] bool intersectsSphere(const glm::vec3& center, float radius) const;
    void update(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
    void update(const glm::vec3& position, const glm::quat& orientation);
    void expandToInclude(const BoundingBox& other);
    void expandToInclude(const glm::vec3& point);
};