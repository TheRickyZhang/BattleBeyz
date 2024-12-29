////////////////////////////////////////////////////////////////////////////////
// BoundingBox.cpp -- Bounding Box object -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BoundingBox.h"
#include "Utils.h"

using namespace std;
/**
* Constructor.
*/
BoundingBox::BoundingBox() :
    min(glm::vec3(1e6)), max(glm::vec3(-1e6))
{
    // 2024-12-03 CAUTION: You must initialize the bounds.  The values set here
    // allow you to immediately check for min/max values of the object that
    // you are bounding.

    setupBoundingBoxBuffers();
}

BoundingBox::BoundingBox(const glm::vec3& min, const glm::vec3& max)
        : min(min), max(max) {
    setupBoundingBoxBuffers();
}

/**
* Destructor.
*/

BoundingBox::~BoundingBox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

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

glm::vec3 BoundingBox::closestPointOutside(const glm::vec3& point) const {
    glm::vec3 adjustedPoint = point;
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

glm::vec3 BoundingBox::closestPointInside(const glm::vec3& point) const {
    glm::vec3 adjustedPoint = point;
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
        std::cout << "Old: " << point.x << " " << point.y << " " << point.z << " " << point.x << " " << point.y << " " << point.z << std::endl;
        std::cout << "New: " << min.x << " " << min.y << " " << min.z << " " << max.x << " " << max.y << " " << max.z << std::endl;
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
* Draw bounding boxes for debugging.
* 
* @param shader                 [in] The shading program.
* 
* @param bodyPosition           [in] Containing object's position.  Used for
*                               bounding box translaton.
*/

void BoundingBox::renderDebug(ShaderProgram &shader, const glm::vec3& bodyPosition) {
    shader.use();
    setupBoundingBoxBuffers();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), bodyPosition);
    shader.setMat4("model", model);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Switch to wireframe mode

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr); // 36 is the number of indices for 12 triangles
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Switch to shaded mode
}

/**
* Set up glfw vertex data.
*/

void BoundingBox::setupBoundingBoxBuffers() {
    // TODO: You need to set this to 1 to align the stadium data, but then it is statdium-specific.
    // Instead, the renderer needs to know the position of it's parent.
    float offset = 0.0f;

    // Vertex data
    float vertices[] = {
        // Positions                    // Normals          // Texture Coords   // Colors (RGB)
        min.x, min.y - offset, min.z,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,         1.0f, 1.0f, 1.0f,
        max.x, min.y - offset, min.z,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,         1.0f, 1.0f, 1.0f,
        max.x, max.y - offset, min.z,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,         1.0f, 1.0f, 1.0f,
        min.x, max.y - offset, min.z,   -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,         1.0f, 1.0f, 1.0f,
        min.x, min.y - offset, max.z,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,         1.0f, 1.0f, 1.0f,
        max.x, min.y - offset, max.z,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,         1.0f, 1.0f, 1.0f,
        max.x, max.y - offset, max.z,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,         1.0f, 1.0f, 1.0f,
        min.x, max.y - offset, max.z,   -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,         1.0f, 1.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0, // Bottom face
        4, 5, 6, 6, 7, 4, // Top face
        0, 1, 5, 5, 4, 0, // Front face
        1, 2, 6, 6, 5, 1, // Right face
        2, 3, 7, 7, 6, 2, // Back face
        3, 0, 4, 4, 7, 3  // Left face
    };

#if 0
    // Debugging active attributes
    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    std::cout << "Active Shader Program ID in BoundingBox: " << currentProgram << std::endl;

    debugActiveAttributes(currentProgram);
    if (!glfwGetCurrentContext()) {
        std::cerr << "No current OpenGL context!" << std::endl;
    }
#endif
    // TOLOOK: Commenting out this code (224-227) causes a crash (probably occured before, openGL errors)
    //showGLErrors("BoundingBox::setupBoundingBoxBuffers");
    setupBuffers(VAO, VBO, EBO, vertices, sizeof(vertices), indices, sizeof(indices), {3, 3, 2, 3});
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
    glm::vec3 center = min + halfSize;

    glm::vec3 newCenter = position;

    min = newCenter - halfSize;
    max = newCenter + halfSize;

    // No angular for now
}
