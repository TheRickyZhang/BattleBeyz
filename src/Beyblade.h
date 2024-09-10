////////////////////////////////////////////////////////////////////////////////
// Beyblade.h -- Beyblade object include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderProgram.h"
#include "Buffers.h"
#include "Texture.h"
#include "GameObject.h"
#include "RigidBody.h"
#include "RigidBodies/BeybladeBody.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include <memory>
#include <iomanip>

class Beyblade : public GameObject {
public:
    Beyblade(std::string modelPath, uint32_t vao, uint32_t vbo, uint32_t ebo,
        BeybladeBody* rigidBody, std::string name);
    ~Beyblade();

    void initializeMesh() override;
    void render(ShaderProgram& shader, const glm::vec3& lightColor, const glm::vec3& lightPos) override;
    BeybladeBody* getRigidBody() { return rigidBody; }

    std::string getName() const { return name; }
    void setName(const std::string& name) { Beyblade::name = name; }
protected:

private:
    std::unordered_map<std::string, glm::vec3> materialColors;
    BeybladeBody* rigidBody;
    std::string modelPath;
    Texture* texture{};
    std::string name;

    void loadModel(const std::string& path);
    void printDebugInfo();
};
