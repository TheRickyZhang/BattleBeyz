////////////////////////////////////////////////////////////////////////////////
// BoundingBox.cpp -- Bounding Box object -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "BoundingBox.h"

using namespace std;
using namespace glm;

// TODO: Change boundary names to mn, mx and remove glm::

/**
* Constructor.
*/
BoundingBox::BoundingBox() : min(vec3(1e6)), max(vec3(-1e6)) {}

BoundingBox::BoundingBox(const vec3& min, const vec3& max) : min(min), max(max) {}

/**
* Check for collision between this bounding box and another.
* 
* @param other                  [in] Other bounding box.
* 
* @return true if thix bounding box and the other bounding box intersect.
*/

bool BoundingBox::intersect(const BoundingBox& a, const BoundingBox& b){
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

vec3 BoundingBox::closestPointOutside(const vec3& point) const {
    vec3 adjustedPoint = point;
    float epsilon = 0.01f; // Small offset to ensure the point is outside
    if (point.x >= max.x) {
        adjustedPoint.x = max.x + epsilon;
    }
    else if (point.x <= min.x) {
        adjustedPoint.x = min.x - epsilon;
    }
    if (point.y >= max.y) {
        adjustedPoint.y = max.y + epsilon;
    }
    else if (point.y <= min.y) {
        adjustedPoint.y = min.y - epsilon;
    }
    if (point.z >= max.z) {
        adjustedPoint.z = max.z + epsilon;
    }
    else if (point.z <= min.z) {
        adjustedPoint.z = min.z - epsilon;
    }
    return adjustedPoint;
}

vec3 BoundingBox::closestPointInside(const vec3& point) const {
    vec3 adjustedPoint = point;
    if (point.x < min.x) {
        adjustedPoint.x = min.x;
    }
    else if (point.x > max.x) {
        adjustedPoint.x = max.x;
    }
    if (point.y < min.y) {
        adjustedPoint.y = min.y;
    }
    else if (point.y > max.y) {
        adjustedPoint.y = max.y;
    }
    if (point.z < min.z) {
        adjustedPoint.z = min.z;
    }
    else if (point.z > max.z) {
        adjustedPoint.z = max.z;
    }
    if (adjustedPoint != point) {
        cout << "Old: " << point.x << " " << point.y << " " << point.z << " " << point.x << " " << point.y << " " << point.z << endl;
        cout << "New: " << min.x << " " << min.y << " " << min.z << " " << max.x << " " << max.y << " " << max.z << endl;
    }
    return adjustedPoint;
}


/**
* Expand a bounding box, basically make a union.
* 
* @param other                  [in] The other bounding box.
*/

void BoundingBox::expandToInclude(const BoundingBox& other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
}

/**
* Expand a bounding box to a point.
* 
* @point                    [in] The 3D point to expand to.
*/

void BoundingBox::expandToInclude(const glm::vec3& point) {
    min = glm::min(min, point);
    max = glm::max(max, point);
}

/**
* Check if this bounding box intersects a sphere.
*
* @param center                 [in] Center of the sphere.
*
* @param radius                 [in] Radius of sphere.
*
* @return true if this bounding box intersects the sphere.
*/

bool BoundingBox::intersectsSphere(const glm::vec3& center, float radius) const {
    float dist_squared = radius * radius;

    if (center.x < min.x) dist_squared -= (center.x - min.x) * (center.x - min.x);
    else if (center.x > max.x) dist_squared -= (center.x - max.x) * (center.x - max.x);

    if (center.y < min.y) dist_squared -= (center.y - min.y) * (center.y - min.y);
    else if (center.y > max.y) dist_squared -= (center.y - max.y) * (center.y - max.y);

    if (center.z < min.z) dist_squared -= (center.z - min.z) * (center.z - min.z);
    else if (center.z > max.z) dist_squared -= (center.z - max.z) * (center.z - max.z);

    return dist_squared > 0;
}

/**
* Update min and max coordinates.
*/

void BoundingBox::update(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    min = glm::min(v1, glm::min(v2, v3));
    max = glm::max(v1, glm::max(v2, v3));
}

/**
* Update position and orientation.
*/

void BoundingBox::update(const glm::vec3& position, const glm::quat& orientation) {

    glm::vec3 halfSize = (max - min) * 0.5f;
    glm::vec3 newCenter = position;

    min = newCenter - halfSize;
    max = newCenter + halfSize;

    // No angular for now
}
