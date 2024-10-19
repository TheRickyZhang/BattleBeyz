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

class Beyblade {
    friend class BeybladeMesh;
    friend class BeybladeBody;
public:
    // Temp: introduce a default constructor for testing purposes
    Beyblade::Beyblade(const std::string& name) : rigidBody(new BeybladeBody()), mesh(new BeybladeMesh()), name(name) {}
    Beyblade::Beyblade(BeybladeBody* rigidBody, BeybladeMesh* mesh, std::string name);
    ~Beyblade();

    void Beyblade::render(ShaderProgram& shader);

    BeybladeBody* getRigidBody() { return rigidBody; }
    BeybladeMesh* getMesh() { return mesh; }

    std::string getName() const { return name; }
    void setName(const std::string& name) { Beyblade::name = name; }
protected:

private:
    BeybladeBody* rigidBody;
    BeybladeMesh* mesh;

    std::string name;
};
