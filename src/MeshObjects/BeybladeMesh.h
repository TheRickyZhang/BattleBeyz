////////////////////////////////////////////////////////////////////////////////
// BeybladMesh.h -- Common game object properties -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "BoundingBox.h"

class ObjectShader;

// Need to call loadModel and updateMesh for using the mesh
class BeybladeMesh {
public:
    BeybladeMesh(std::string& modelPath, unsigned int vao, unsigned int vbo, unsigned int ebo, glm::vec3& tint = glm::vec3(1.0f))
        : modelPath(std::move(modelPath)), VAO(vao), VBO(vbo), EBO(ebo), tint(tint) {
        updateMesh();
    }
    BeybladeMesh(const char* path = "./assets/models/default.obj") : modelPath(path), VAO(0), VBO(0), EBO(0), tint(glm::vec3(1.0f)) {
        updateMesh();
    }

    bool loadModel(const std::string& path);
    const std::string& getModelPath() const { return modelPath; }
    void printDebugInfo();

    //unsigned int getVAO() const { return VAO; }
    //int getIndicesSize() { return static_cast<int>(indices.size()); }
    //std::unordered_map<std::string, glm::vec3>& getMaterialColors() { return materialColors; }

    void render(ObjectShader& shader);

    BoundingBox boundingBox{};                          // Mesh bounding box.
    float heightDisc{}, heightLayer{}, heightDriver{};  // Heights of subparts
    float radiusDisc{}, radiusLayer{}, radiusDriver{};  // Radii of subparts

    bool modelLoaded = false;                           // True if loadModel succeeded.

    glm::vec3 tint;   // Apply tint to entire beyblade, where white = no affect.
                      // Could be useful for special move / low health / team battle indicator
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> tangents;

    std::unordered_map<std::string, glm::vec3> materialColors;
    std::string modelPath;

    std::vector<glm::vec3> colors;
    std::vector<float> vertexData;

    unsigned int VAO{}, VBO{}, EBO{};

    void updateMesh();
};