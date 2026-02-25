////////////////////////////////////////////////////////////////////////////////
// BoundingBoxRenderer.cpp -- Debug drawing for BoundingBox -- rz -- 2026-08-25
////////////////////////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "BoundingBoxRenderer.h"

#include "BoundingBox.h"
#include "Buffers.h"
#include "ObjectShader.h"

namespace {
    constexpr int VERTEX_COUNT = 8;
    constexpr int FLOATS_PER_VERTEX = 11;  // pos3, normal3, uv2, color3
    constexpr int INDEX_COUNT = 36;

    const unsigned int BOX_INDICES[INDEX_COUNT] = {
        0, 1, 2, 2, 3, 0, // Bottom face
        4, 5, 6, 6, 7, 4, // Top face
        0, 1, 5, 5, 4, 0, // Front face
        1, 2, 6, 6, 5, 1, // Right face
        2, 3, 7, 7, 6, 2, // Back face
        3, 0, 4, 4, 7, 3  // Left face
    };
}

BoundingBoxRenderer::BoundingBoxRenderer() = default;

BoundingBoxRenderer::~BoundingBoxRenderer() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void BoundingBoxRenderer::ensureBuffers() {
    if (VAO) return;

    float vertices[VERTEX_COUNT * FLOATS_PER_VERTEX] = {};
    setupBuffers(VAO, VBO, EBO, vertices, sizeof(vertices), BOX_INDICES, sizeof(BOX_INDICES),
        { 3, 3, 2, 3 });
}

void BoundingBoxRenderer::begin(ObjectShader& shader) {
    ensureBuffers();
    shader.use();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void BoundingBoxRenderer::draw(ObjectShader& shader, const BoundingBox& box, const glm::vec3& bodyPosition) {
    ensureBuffers();

    const glm::vec3& mn = box.min;
    const glm::vec3& mx = box.max;
    const float corners[VERTEX_COUNT][3] = {
        { mn.x, mn.y, mn.z }, { mx.x, mn.y, mn.z }, { mx.x, mx.y, mn.z }, { mn.x, mx.y, mn.z },
        { mn.x, mn.y, mx.z }, { mx.x, mn.y, mx.z }, { mx.x, mx.y, mx.z }, { mn.x, mx.y, mx.z }
    };

    float vertices[VERTEX_COUNT * FLOATS_PER_VERTEX] = {};
    for (int i = 0; i < VERTEX_COUNT; ++i) {
        float* v = vertices + i * FLOATS_PER_VERTEX;
        v[0] = corners[i][0];  v[1] = corners[i][1];  v[2] = corners[i][2];
        v[4] = 1.0f;                                    // Normal, unused in wireframe
        v[8] = v[9] = v[10] = 1.0f;                     // White
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    shader.setObjectRenderParams(glm::translate(glm::mat4(1.0f), bodyPosition), glm::vec3(1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, INDEX_COUNT, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void BoundingBoxRenderer::end() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
