////////////////////////////////////////////////////////////////////////////////
// QuadRenderer.cpp -- Quad Renderer -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include <vector>

#include "QuadRenderer.h"
#include "Buffers.h"
#include "ShaderProgram.h"

using namespace glm;
/**
 * Constructor
 */
QuadRenderer::QuadRenderer(const glm::mat4& model, const glm::mat4& projection)
    : quadVAO(0), quadVBO(0), quadEBO(0), modelMatrix(model), projectionMatrix(projection) {
    float quadVertices[] = {
        // positions        // texture coords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f
    };
    setupBuffers(quadVAO, quadVBO, quadEBO, quadVertices, sizeof(quadVertices), nullptr, 0, { 3, 2 });
}


/**
 * Destructor.
 */
QuadRenderer::~QuadRenderer() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadEBO); // Optional, but safe
}

void QuadRenderer::setModelMatrix(const glm::mat4& model) {
    modelMatrix = model;
}

void QuadRenderer::setProjectionMatrix(const glm::mat4& projection) {
    projectionMatrix = projection;
}
/**
 * Render the object with the given shader.
 */
void QuadRenderer::render(ShaderProgram& shader) const {
    shader.use();
    shader.setMat4("model", modelMatrix);
    shader.setMat4("projection", projectionMatrix);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6); // Draw 6 vertices (2 triangles)
    glBindVertexArray(0);
}