////////////////////////////////////////////////////////////////////////////////
// Utils.h -- Utility routines include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#define GL_CHECK(message, ...) do { \
        __VA_ARGS__; \
        checkGLError(#message, __FILE__, __LINE__); \
    } while (0)

void checkGLError(const char* stmt, const char* fname, int line);
std::string checkIntersection(const glm::vec3 & ray_world);
void printVec3(const std::string& label, const glm::vec3& v);
glm::vec3 screenToWorldCoordinates(GLFWwindow * window, float xpos, float ypos, const glm::mat4 & view, const glm::mat4 & projection);

int floatToDiscreteInt(float val, float mn, float mx);
float discreteIntToFloat(int val, float mn, float mx);

std::ostream& operator<<(std::ostream & os, const glm::mat4 & mat);

class MockLoading {
public:
    static MockLoading& getInstance() {
        static MockLoading instance;
        return instance;
    }

    void mock(){
        for (long long  i = 0; i < 5e8; ++i) {
            cnt++;
        }
    }

    long long cnt = 0;

    MockLoading() = default;
    ~MockLoading() = default;
    MockLoading(const MockLoading&) = delete;
    MockLoading& operator=(const MockLoading&) = delete;
};


