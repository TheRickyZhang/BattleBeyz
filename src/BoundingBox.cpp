////////////////////////////////////////////////////////////////////////////////
// BoundingBox.cpp -- Bounding Box object -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BoundingBox.h"

/**
* Constructor.
*/

BoundingBox::BoundingBox()
        : min(glm::vec3(FLT_MAX)), max(glm::vec3(-FLT_MAX)), VAO(0), VBO(0), EBO(0) {
    setupBuffers();
}

/**
* Alternate constructor.
*/

BoundingBox::BoundingBox(const glm::vec3& min, const glm::vec3& max)
        : min(min), max(max), VAO(0), VBO(0), EBO(0) {
    setupBuffers();
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

bool BoundingBox::checkCollision(const BoundingBox &other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y) &&
           (min.z <= other.max.z && max.z >= other.min.z);
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

    setupBuffers();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), bodyPosition);
    shader.setUniformMat4("model", model);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Switch to wireframe mode

    //glm::vec3 color(0.0f, 1.0f, 1.0f);  // Set uniform for object color to white.
    //shader.setObjectColor(&color);

    // TODO: How about quads instead of triangles?
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr); // 36 is the number of indices for 12 triangles
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Switch to shaded mode

    //shader.setObjectColor(nullptr);  // Now disable this feature

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}

/**
* Set up glfw vertex data.
*/

void BoundingBox::setupBuffers() {
    // TODO: You need to set this to 1 to align the stadium data, but then it is statdium-specific.
    // Instead, the renderer needs to know the position of it's parent.
    float offset = 0.0f;

    //    float vertices[] = {
    //            // Positions         // Normals         // Texture Coords  // Colors (RGB)
    //            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,      1.0f, 0.0f, 0.0f, // Red
    //            1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,      0.0f, 1.0f, 0.0f, // Green
    //            1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,      0.0f, 0.0f, 1.0f, // Blue
    //            -1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,      1.0f, 1.0f, 0.0f, // Yellow
    //            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,      1.0f, 0.0f, 1.0f, // Magenta
    //            1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,      0.0f, 1.0f, 1.0f, // Cyan
    //            1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,      1.0f, 0.5f, 0.0f, // Orange
    //            -1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,      0.5f, 0.5f, 0.5f  // Grey
    //    };
    //
    //    unsigned int indices[] = {
    //            0, 1, 2, 2, 3, 0, // Bottom face
    //            4, 5, 6, 6, 7, 4, // Top face
    //            0, 1, 5, 5, 4, 0, // Front face
    //            1, 2, 6, 6, 5, 1, // Right face
    //            2, 3, 7, 7, 6, 2, // Back face
    //            3, 0, 4, 4, 7, 3  // Left face
    //    };

#if 0
    float vertices[] = {
        // Positions         // Normals         // Texture Coords  // Colors (RGB)
        min.x, min.y - offset, min.z,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,      1.0f, 0.0f, 0.0f, // Red
        max.x, min.y - offset, min.z,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,      0.0f, 1.0f, 0.0f, // Green
        max.x, max.y - offset, min.z,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,      0.0f, 0.0f, 1.0f, // Blue
        min.x, max.y - offset, min.z, -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,      1.0f, 1.0f, 0.0f, // Yellow
        min.x, min.y - offset, max.z,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,      1.0f, 0.0f, 1.0f, // Magenta
        max.x, min.y - offset, max.z,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,      0.0f, 1.0f, 1.0f, // Cyan
        max.x, max.y - offset, max.z,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,      1.0f, 0.5f, 0.0f, // Orange
        min.x, max.y - offset, max.z, -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,      0.5f, 0.5f, 0.5f  // Grey
    };
#else  // Changed all colors to white
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
#endif

    unsigned int indices[] = {
            0, 1, 2, 2, 3, 0, // Bottom face
            4, 5, 6, 6, 7, 4, // Top face
            0, 1, 5, 5, 4, 0, // Front face
            1, 2, 6, 6, 5, 1, // Right face
            2, 3, 7, 7, 6, 2, // Back face
            3, 0, 4, 4, 7, 3  // Left face
    };

    ::setupBuffers(VAO, VBO, EBO, vertices, sizeof(vertices), indices, sizeof(indices));
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
//    glm::vec3 halfSize = (max - min) * 0.5f;
//    glm::vec3 localCenter = min + halfSize;
//
//    // Transform local center to new center position
//    glm::vec3 newCenter = position + glm::rotate(orientation, localCenter - position);
//
//    // Rotate the half-size vector to account for the bounding box orientation
//    glm::vec3 rotatedHalfSize = glm::abs(glm::rotate(orientation, halfSize));
//
//    min = newCenter - rotatedHalfSize;
//    max = newCenter + rotatedHalfSize;
}
