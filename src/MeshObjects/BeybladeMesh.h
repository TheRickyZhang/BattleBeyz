////////////////////////////////////////////////////////////////////////////////
// BeybladMesh.h -- Common game object properties -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <unordered_map>
#include <iomanip>
#include <glm/gtx/quaternion.hpp>

#include "ShaderProgram.h"
#include "Buffers.h"
#include "Texture.h"
#include "RigidBodies/StadiumBody.h"

// Need to call loadModel and initializeMesh for using the mesh
class BeybladeMesh {
    //friend class Beyblade;
public:
    // TODO: Remove color?
    BeybladeMesh(std::string& modelPath, unsigned int vao, unsigned int vbo, unsigned int ebo, const glm::vec3& col)
        : modelPath(std::move(modelPath)), VAO(vao), VBO(vbo), EBO(ebo), color(col) {
        initializeMesh();
    }
    BeybladeMesh(const char* path = "./assets/models/default.obj") : modelPath(path), VAO(0), VBO(0), EBO(0), color(glm::vec3(1.0f)) {
        initializeMesh();
    }

    bool loadModel(const std::string& path);
    const std::string& getModelPath() const { return modelPath; }
    void printDebugInfo();

    unsigned int getVAO() const { return VAO; }
    int getIndicesSize() { return static_cast<int>(indices.size()); }
    std::unordered_map<std::string, glm::vec3>& getMaterialColors() { return materialColors; }

    BoundingBox boundingBox{};                          // Mesh bounding box.
    float heightDisc{}, heightLayer{}, heightDriver{};  // Heights of subparts
    float radiusDisc{}, radiusLayer{}, radiusDriver{};  // Radii of subparts

    bool modelLoaded = false;                           // True if loadModel succeeded.
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
    glm::vec3 color;

    void initializeMesh();
};