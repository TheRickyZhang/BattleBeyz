////////////////////////////////////////////////////////////////////////////////
// Utils.cpp -- Utility routines -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "Utils.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include "BoundingBox.h"

#include <glm/gtc/matrix_access.hpp>


/**
* Convert screen to world coordinates.
* 
* @param xpos                   [in] X position in screen coordinates.
* 
* @param ypos                   [in] Y position in screen coordinates.
* 
* @param view                   [in] View matrix.
* 
* @param projection             [in] Projeciton matrix.
* 
* @returns 3D world coordinates.
*/

glm::vec3 screenToWorldCoordinates(GLFWwindow* window, float xpos, float ypos, const glm::mat4& view, const glm::mat4& projection) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float x = (2.0f * (float)xpos) / width - 1.0f;
    float y = 1.0f - (2.0f * (float)ypos) / height;
    float z = 1.0f;

    glm::vec3 ray_nds = glm::vec3(x, y, z);
    glm::vec4 ray_clip = glm::vec4(ray_nds, 1.0f);
    glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

    glm::vec3 ray_wor = glm::vec3(glm::inverse(view) * ray_eye);
    ray_wor = glm::normalize(ray_wor);

    return ray_wor;
}


/**
 * Returns true if the ray intersects the axis aligned bounding box.
 *
 * @param rayOrigin  [in]  The vec3 point the viewing ray originates from.
 * @param rayDir     [in]  A non-zero vector representing the ray's direction.
 * @param box        [in]  The bounding box with public 'min' and 'max' attributes.
 * @param tNear      [out] The distance along the ray to the intersection point.
 * @return           True if an intersection occurs, false otherwise.
 */
bool rayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
    const BoundingBox& box, float& tNear)
{
    const float epsilon = 1e-6f;
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    // X-axis slab
    if (std::fabs(rayDir.x) < epsilon) {
        if (rayOrigin.x < box.min.x || rayOrigin.x > box.max.x)
            return false;
        tmin = -FLT_MAX;
        tmax = FLT_MAX;
    }
    else {
        tmin = (box.min.x - rayOrigin.x) / rayDir.x;
        tmax = (box.max.x - rayOrigin.x) / rayDir.x;
        if (tmin > tmax) std::swap(tmin, tmax);
    }

    // Y-axis slab
    if (std::fabs(rayDir.y) < epsilon) {
        if (rayOrigin.y < box.min.y || rayOrigin.y > box.max.y)
            return false;
        tymin = -FLT_MAX;
        tymax = FLT_MAX;
    }
    else {
        tymin = (box.min.y - rayOrigin.y) / rayDir.y;
        tymax = (box.max.y - rayOrigin.y) / rayDir.y;
        if (tymin > tymax) std::swap(tymin, tymax);
    }

    if ((tmin > tymax) || (tymin > tmax))
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    // Z-axis slab
    if (std::fabs(rayDir.z) < epsilon) {
        if (rayOrigin.z < box.min.z || rayOrigin.z > box.max.z)
            return false;
        tzmin = -FLT_MAX;
        tzmax = FLT_MAX;
    }
    else {
        tzmin = (box.min.z - rayOrigin.z) / rayDir.z;
        tzmax = (box.max.z - rayOrigin.z) / rayDir.z;
        if (tzmin > tzmax) std::swap(tzmin, tzmax);
    }

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    tNear = tmin;
    return true;
}

/**
* Perform intersection test with objects in your scene and return the object name.
*/

std::string checkIntersection(const glm::vec3& ray_world) {
    if (glm::length(ray_world - glm::vec3(0.0f, 0.0f, 0.0f)) < 1.0f) {
        return "Object1";
    }
    return "None";
}

/**
* Print an error message if an OpenGL error occured.
*/

void checkGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << err << " at " << fname << ":" << line << " for " << stmt << std::endl;
    }
}

/**
* Print a 3D vector for debugging.
* 
* @param label                  [in] A prefix message.
* 
* @param v                      [in] Vector to print.
*/

void printVec3(const std::string& label, const glm::vec3& v) {
    std::cout << label << ": " << v.x << ", " << v.y << ", " << v.z << std::endl;
}

std::ostream& operator<<(std::ostream& os, const glm::mat4& mat) {
    for (int row = 0; row < 4; ++row) {
        glm::vec4 r = glm::row(mat, row); // Access the row
        os << "| " << r.x << ", " << r.y << ", " << r.z << ", " << r.w << " |" << std::endl;
    }
    return os;
}

int floatToDiscreteInt(float val, float mn, float mx) {
    return std::clamp(static_cast<int>(std::round(1.0f + ((val - mn) / (mx - mn)) * 9.0f)), 1, 10);
}

float discreteIntToFloat(int val, float mn, float mx) {
    return mn + ((val - 1) / 9.0f) * (mx - mn);
}