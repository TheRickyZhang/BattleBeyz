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

// Note this beyblade id is GLOBALLY UNIQUE
class Beyblade {
    friend class BeybladeMesh;
    friend class BeybladeBody;
public:
    Beyblade(int id, const std::string& name) :
        id(id), rigidBody(new BeybladeBody()), mesh(new BeybladeMesh()), name(name) {}
    Beyblade(int id, std::string name, BeybladeBody* rigidBody, BeybladeMesh* mesh) :
        id(id), name(name), rigidBody(rigidBody), mesh(mesh) {}
    ~Beyblade();

    void render(ShaderProgram& shader);

    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string setName() const { return name; }
    BeybladeBody* getRigidBody() { return rigidBody; }
    BeybladeMesh* getMesh() { return mesh; }

    void setName(const std::string& name) { Beyblade::name = name; }
protected:

private:
    const int id; // Globally unique, will be managed by centralized server
    std::string name;
    BeybladeBody* rigidBody;
    BeybladeMesh* mesh;
};
