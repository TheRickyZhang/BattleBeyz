#include <iostream>

#include "QuadRenderer.h"

#include "Utils.h"

QuadRenderer::QuadRenderer(const glm::mat4& model)
    : modelMatrix(model) {
    setupBuffers();
}

QuadRenderer::~QuadRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void QuadRenderer::setupBuffers() {
    float quadVertices[] = {
        // positions       // tex coords
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // Position (location=0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        // Texture coords (location=1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(0);
}

void QuadRenderer::setModelMatrix(const glm::mat4& model) {
    modelMatrix = model;
}

void QuadRenderer::render(BackgroundShader& shader, std::shared_ptr<Texture> texture) const {
    if (!texture) {
        std::cerr << "Warning: QuadRenderer received a null texture.\n";
        return;
    }

    shader.use();
    shader.setBackgroundObjectParams(modelMatrix, 0);

    texture->use();

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    GL_CHECK("Quadrenderer");
}
