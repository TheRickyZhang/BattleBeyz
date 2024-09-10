////////////////////////////////////////////////////////////////////////////////
// Utils.h -- Utility routines include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stb_image.h>

// Macro to wrap OpenGL calls for error checking
#define GL_CHECK(stmt) do { \
        stmt; \
        checkGLError(#stmt, __FILE__, __LINE__); \
    } while (0)


glm::vec3 screenToWorldCoordinates(GLFWwindow* window, double xpos, double ypos, const glm::mat4& view, const glm::mat4& projection);
glm::vec3 dv3(double d);
glm::vec3 getVecFromMagnitude(double magnitude, glm::vec3 vector3);
std::string checkIntersection(const glm::vec3& ray_world);
void checkGLError(const char* stmt, const char* fname, int line);
void cleanup(GLFWwindow* window);
void printVec3(const std::string& label, const glm::vec3& v);

enum ProgramState {
    LOADING,
    RENDERING,
    ACTIVE
};

/**
* Clear way to manage angular and linear changes in Acceleration calculated in PhysicsWorld.update()
* 
* Note: inherent Acceleration of beyblade should be taken into account into future (suppose we have power-up that accelarates (?))
* BUT it should always be 0 in most cases because instantaneous Accelerations are all calculated and applied each deltaTime.
*/
struct CollisionAccelerations {
    glm::vec3 angularAcceleration = glm::vec3(0.0f);
    glm::vec3 linearAcceleration = glm::vec3(0.0f);

    void add(const std::pair<glm::vec3, glm::vec3>& increment) {
        angularAcceleration += increment.first;
        linearAcceleration += increment.second;
    }
};
