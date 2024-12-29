////////////////////////////////////////////////////////////////////////////////
// Buffers.cpp -- GLFW vertex data handling -- rz -- 2024-08-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#include "Buffers.h"
#include "Utils.h"

/**
* Set up the vertex data and indices.  This variation requires that colors be embedded
* in the vertex data.
* 
* Refer to OpenGL documentation for detailed descriptions of the various objects.
* 
* @param VAO                    [in] OpenGL Vertex Array Object
* 
* @param VBO                    [in] OpenGL Vertex Buffer Object.
* 
* @param EBO                    [in] OpenGL Element Buffer Object.
* 
* @param vertices               [in] Vertices.
* 
* @param verticesSize           [in] Number of vertices.
* 
* @param indices                [in] Vertex indices.
* 
* @param indicesSize            [in] Number of vertex indices.
*/

void setupBuffers(
    unsigned int& VAO,
    unsigned int& VBO,
    unsigned int& EBO,
    const float* vertices,
    size_t verticesSize,
    const unsigned int* indices,
    size_t indicesSize,
    const std::vector<int>& attributeSizes
) {
    // ERRORS
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

    // Calculate stride
    int stride = 0;
    for (int size : attributeSizes) {
        stride += size;
    }
    stride *= sizeof(float);

    // Set attributes
    uintptr_t offset = 0;
    for (size_t i = 0; i < attributeSizes.size(); ++i) {
        GLuint index = static_cast<GLuint>(i);
        glVertexAttribPointer(index, attributeSizes[i], GL_FLOAT, GL_FALSE, stride, (void*)offset);
        glEnableVertexAttribArray(index);
        offset += attributeSizes[i] * sizeof(float);
    }

    glBindVertexArray(0);
}