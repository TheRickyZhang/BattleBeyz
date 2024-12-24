////////////////////////////////////////////////////////////////////////////////
// QuadRenderer.cpp -- Quad Renderer -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "QuadRenderer.h"
#include "Utils.h"
#include <vector>
#include "Buffers.h"

/**
 * Constructor.
 */
QuadRenderer::QuadRenderer(float vecScale) : quadVAO(0), quadVBO(0), quadEBO(0) {
    float quadVertices[] = {
        // positions        // texture coords
        -vecScale,  vecScale, 0.0f,  0.0f, 1.0f,
        -vecScale, -vecScale, 0.0f,  0.0f, 0.0f,
        vecScale, -vecScale, 0.0f,  1.0f, 0.0f,

        -vecScale,  vecScale, 0.0f,  0.0f, 1.0f,
        vecScale, -vecScale, 0.0f,  1.0f, 0.0f,
        vecScale,  vecScale, 0.0f,  1.0f, 1.0f
    };

    std::vector<int> attributeSizes = { 3, 2 }; // 3 floats for position, 2 for texture coordinates
    setupBuffers(quadVAO, quadVBO, quadEBO, quadVertices, sizeof(quadVertices), nullptr, 0, attributeSizes);
}

/**
 * Destructor.
 */
QuadRenderer::~QuadRenderer() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadEBO); // Optional, but safe
}

/**
 * Render the object with the given shader.
 */
void QuadRenderer::render(ShaderProgram& shader) const {
    shader.use(); // Use the shader program
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6); // Draw 6 vertices (2 triangles)
    glBindVertexArray(0);
}