////////////////////////////////////////////////////////////////////////////////
// Buffers.h -- Buffers class include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

// 12/24/24: ONLY NEED 1 call, pass in sizes of attributes
void setupBuffers(
    unsigned int& VAO,
    unsigned int& VBO,
    unsigned int& EBO,
    const float* vertices,
    size_t verticesSize,
    const unsigned int* indices,
    size_t indicesSize,
    const std::vector<int>& attributeSizes
);

// Takes in Vertex Array Object, Vertex Buffer Object, Element Buffer Object, vertices, and indices
// Binds the objects such that the vertex attributes are stored as 3 pos, 3 color floats
//void setupBuffers(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO,
//                  const float* vertices, size_t verticesSize,
//                  const unsigned int* indices, size_t indicesSize);
//
//void setupBuffersTexturesOnly(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO, const float* vertices,
//                  size_t verticesSize, const unsigned int* indices, size_t indicesSize);