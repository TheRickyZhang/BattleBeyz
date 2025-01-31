#include <iostream>

#include "QuadRenderer.h"
#include "imgui.h" // TEmporary

#include "Utils.h"

QuadRenderer::QuadRenderer() {
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

void QuadRenderer::render(BackgroundShader& shader, std::shared_ptr<Texture> texture) const {
    if (!texture || texture->ID == 0) {
        std::cerr << "Warning: QuadRenderer received a null or invalid texture.\n";
        return;
    }

    // DEBUGGING
    // Force absolute values to shader
    //glViewport(0, 0, 800, 800);
    //int vp[4];
    //glGetIntegerv(GL_VIEWPORT, vp);
    //std::cerr << "glViewport: " << vp[0] << ", " << vp[1] << ", " << vp[2] << ", " << vp[3] << "\n";

    shader.setBackgroundObjectParams(glm::mat4(1.0f), 0);

    texture->use(0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
