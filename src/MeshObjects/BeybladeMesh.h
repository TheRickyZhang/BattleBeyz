////////////////////////////////////////////////////////////////////////////////
// BeybladMesh.h -- Common game object properties -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "BoundingBox.h"
#include "MeshData.h"

class ObjectShader;

/*
* GPU-side beyblade model. Construction uploads already-parsed geometry; run
* parseObj() to produce it, which needs no GL context and so can run on a worker.
*/
class BeybladeMesh {
public:
    explicit BeybladeMesh(const MeshData& data, const glm::vec3& tint = glm::vec3(1.0f));
    ~BeybladeMesh();

    // Owns GL handles that the destructor deletes
    BeybladeMesh(const BeybladeMesh&) = delete;
    BeybladeMesh& operator=(const BeybladeMesh&) = delete;

    const std::string& getModelPath() const { return modelPath; }
    void printDebugInfo();

    void render(ObjectShader& shader);

    BoundingBox boundingBox{};                          // Mesh bounding box.
    float heightDisc{}, heightLayer{}, heightDriver{};  // Heights of subparts
    float radiusDisc{}, radiusLayer{}, radiusDriver{};  // Radii of subparts

    bool modelLoaded = false;                           // True if the source parse succeeded.

    glm::vec3 tint;   // Apply tint to entire beyblade, where white = no affect.
                      // Could be useful for special move / low health / team battle indicator
private:
    void upload(const MeshData& data);

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> colors;

    std::string modelPath;

    unsigned int VAO{}, VBO{}, EBO{};
};
