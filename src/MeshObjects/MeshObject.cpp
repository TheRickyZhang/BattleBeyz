#include "MeshObject.h"
#include "Buffers.h"

#include "Utils.h" // DEBUGGING

using namespace std;

MeshObject::MeshObject() : VAO(0), VBO(0), EBO(0) {}

MeshObject::~MeshObject() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void MeshObject::setupBuffersFromMembers() {
    std::vector<float> vertexData;
    for (size_t i = 0; i < vertices.size(); ++i) {
        // Position (x, y, z)
        vertexData.push_back(vertices[i].x);
        vertexData.push_back(vertices[i].y);
        vertexData.push_back(vertices[i].z);

        // Normal (x, y, z) - Ensure normals are populated in the derived class
        if (i < normals.size()) {
            vertexData.push_back(normals[i].x);
            vertexData.push_back(normals[i].y);
            vertexData.push_back(normals[i].z);
        }
        else {
            vertexData.insert(vertexData.end(), { 0.0f, 0.0f, 0.0f });
        }

        // Texture Coordinates (u, v)
        if (i < texCoords.size()) {
            vertexData.push_back(texCoords[i].x);
            vertexData.push_back(texCoords[i].y);
        }
        else {
            vertexData.insert(vertexData.end(), { 0.0f, 0.0f });
        }

        // **Colors (r, g, b)**
        if (i < colors.size()) {
            vertexData.push_back(colors[i].x);
            vertexData.push_back(colors[i].y);
            vertexData.push_back(colors[i].z);
        }
        else {
            vertexData.insert(vertexData.end(), { 1.0f, 1.0f, 1.0f }); // Default white color
        }
    }

    setupBuffers(
        VAO, VBO, EBO,
        vertexData.data(), vertexData.size() * sizeof(float),
        indices.data(), indices.size() * sizeof(unsigned int),
        { 3, 3, 2, 3 } // 3 for position, 3 for normal, 2 for texCoords, 3 for color
    );
}


void MeshObject::render(ObjectShader& shader, std::shared_ptr<Texture> texture) {
    shader.use();
    shader.setObjectRenderParams(modelMatrix, tint);
    //shader.setVec2("textureScale", textureScale);   // If you need to scale the texture

    if (texture) {
        texture->use();                    // Bind texture if available
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, GLsizei(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    GL_CHECK("MeshObject");
}


// Getters and Setters
void MeshObject::setModelMatrix(const glm::mat4& newModel) {
    modelMatrix = newModel;
}

const glm::mat4& MeshObject::getModelMatrix() const {
    return modelMatrix;
}

void MeshObject::setTint(const glm::vec3& newTint) {
    tint = newTint;
}

//void MeshObject::setTextureScale(const glm::vec2& scale) {
//    textureScale = scale;
//}
//
//const glm::vec2& MeshObject::getTextureScale() const {
//    return textureScale;
//}
