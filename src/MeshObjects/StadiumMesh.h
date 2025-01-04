////////////////////////////////////////////////////////////////////////////////
// MeshObject,h -- Common game object properties -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Texture.h"

class Beyblade;

// Note - radius is not stored anywhere here. I think the mesh data already indicates its size?
class StadiumMesh {
    friend class Stadium;
public:
    StadiumMesh(Texture* texture, int verticesPerRing, int numRings, const glm::vec3& ringColor, const glm::vec3& crossColor, const glm::vec3& col, float textureScale)
        : texture(texture), verticesPerRing(verticesPerRing), numRings(numRings),
        ringColor(ringColor), crossColor(crossColor), color(col), textureScale(textureScale) {}
    StadiumMesh();

    Texture* getTexture() { return texture; }

    //void printDebugInfo();
protected:
    Texture* texture{};

    // Mesh data
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> tangents;

    std::vector<glm::vec3> colors;
    std::vector<float> vertexData;

    int verticesPerRing;
    int numRings;
    glm::vec3 ringColor;
    glm::vec3 crossColor;

    float textureScale = 1.0f;

    unsigned int VAO{}, VBO{}, EBO{};
    glm::vec3 color;

};