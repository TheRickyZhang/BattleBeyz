////////////////////////////////////////////////////////////////////////////////
// GameObject,h -- Common game object properties -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

class ShaderProgram;

class GameObject {
public:
    GameObject(unsigned int vao, unsigned int vbo, unsigned int ebo, const glm::vec3& col)
            : VAO(vao), VBO(vbo), EBO(ebo), color(col) {}

    // Pure virtual functions
    virtual void initializeMesh() = 0;
    virtual void render(ShaderProgram &shader, const glm::vec3 &lightColor, const glm::vec3 &lightPos) = 0;
protected:
    // Mesh data
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> tangents;
    
    std::vector<glm::vec3> colors;
    std::vector<float> vertexData;

    unsigned int VAO, VBO, EBO;
    glm::vec3 color;
};