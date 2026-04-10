////////////////////////////////////////////////////////////////////////////////
// MeshObject,h -- Common game object properties -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "ObjectShader.h"
#include "Texture.h"

class MeshObject {
public:
    MeshObject();
    virtual ~MeshObject() noexcept;

    // Owns raw GL handles, so copying would double-delete them. Move transfers
    // ownership and zeroes the source. Needed because Stadium::fromJson returns by value.
    MeshObject(const MeshObject&) = delete;
    MeshObject& operator=(const MeshObject&) = delete;
    MeshObject(MeshObject&& other) noexcept;
    MeshObject& operator=(MeshObject&& other) noexcept;

    virtual void updateMesh() = 0;

    // Not virtual: nothing holds a MeshObject base pointer, and Stadium deliberately
    // hides this behind a one-argument form that supplies its own texture.
    void render(ObjectShader& shader, Texture* texture = nullptr);

    // Getters and setters
    void setModelMatrix(const glm::mat4& newModel);
    const glm::mat4& getModelMatrix() const;

    glm::vec3 getTint() const { return tint; }
    void setTint(const glm::vec3& newTint);
    //void setTextureScale(const glm::vec2& scale);
    //const glm::vec2& getTextureScale() const;

protected:
    unsigned int VAO=0, VBO=0, EBO=0;

    std::vector<glm::vec3> vertices;   // Position (x, y, z)
    std::vector<glm::vec3> normals;    // Normal (x, y, z)
    std::vector<glm::vec2> texCoords;  // Texture coordinates (u, v)
    std::vector<glm::vec3> colors;     // **Changed from tangents to colors**
    std::vector<unsigned int> indices; // Indices for indexed drawing

    // Local Variables
    
    // Only set on initialization; assumed static if the object does not move
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec3 tint = glm::vec3(1.0f);

    //glm::vec2 textureScale = glm::vec2(1.0f, 1.0f);

    void setupBuffersFromMembers();
};
