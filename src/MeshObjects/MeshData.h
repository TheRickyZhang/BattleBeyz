////////////////////////////////////////////////////////////////////////////////
// MeshData.h -- CPU-side parsed model -- rz -- 2026-08-25
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "BoundingBox.h"

/*
* Geometry parsed out of an .obj, with no OpenGL attached. parseObj() produces one of
* these off the GL thread; BeybladeMesh uploads it on the thread holding the context.
*/
struct MeshData {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> colors;
    std::vector<unsigned int> indices;
    std::unordered_map<std::string, glm::vec3> materialColors;

    BoundingBox boundingBox{};
    float heightDisc{}, heightLayer{}, heightDriver{};  // Heights of subparts
    float radiusDisc{}, radiusLayer{}, radiusDriver{};  // Radii of subparts

    std::string modelPath;
    bool loaded = false;
};

// File read plus OBJ/MTL parse. No OpenGL, safe to call from any thread.
MeshData parseObj(const std::string& path);

constexpr const char* DEFAULT_MODEL_PATH = "./assets/models/default.obj";
