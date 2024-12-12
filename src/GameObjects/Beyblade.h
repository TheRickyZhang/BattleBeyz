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
#include "MeshObjects/BeybladeMesh.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include <memory>
#include <iomanip>
#include "BeybladeTemplate.h"
#include "Utils.h"
#include <stdexcept>

class Beyblade {
    friend class BeybladeMesh;
    friend class BeybladeBody;
public:
    // Note this beyblade id is GLOBALLY UNIQUE
    Beyblade(int id, const std::string& name, bool isTemplate = false);
    //Beyblade(int id, std::string name, BeybladeBody* rigidBody, BeybladeMesh* mesh) :
    //    id(id), name(name), rigidBody(rigidBody), mesh(mesh), isTemplate(true) {}

    void render(ShaderProgram& shader);

    int getId() const { return id; }
    std::string getName() const { return name; }
    BeybladeBody* getRigidBody() { return rigidBody.get(); }
    BeybladeMesh* getMesh() { return mesh.get(); }
    void setName(const std::string& name) { Beyblade::name = name; }

    void update(int layerIndex, int discIndex, int driverIndex);

    bool isTemplate = false;
    int templateIndices[3] = { -1, -1, -1 };  // ONLY used by templated beyblades

protected:

private:
    const int id; // Globally unique, will be managed by centralized server
    std::string name;
    std::unique_ptr<BeybladeBody> rigidBody;
    std::unique_ptr<BeybladeMesh> mesh;

    void setTemplateIndices(int layerIndex, int discIndex, int driverIndex);
};
