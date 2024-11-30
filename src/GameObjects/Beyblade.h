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
    Beyblade(int id, const std::string& name, bool isTemplate = false) :
        id(id), name(name), isTemplate(isTemplate) {
        if (isTemplate) {
            setTemplateIndices(0, 0, 0);
            rigidBody = new BeybladeBody(templateLayers[0].part, templateDiscs[0].part, templateDrivers[0].part);
            // TODO: Implement a combineMesh function. Requires all obj files to be standardized, centered and aligned correctly. Then combine them
            mesh = new BeybladeMesh();
            //mesh = combineMesh(templateLayers[0].modelPath, templateDiscs[0].modelPath, templateDrivers[0].modelPath);
        }
        else {
            rigidBody = new BeybladeBody();
            mesh = new BeybladeMesh();
        }
    }
    //Beyblade(int id, std::string name, BeybladeBody* rigidBody, BeybladeMesh* mesh) :
    //    id(id), name(name), rigidBody(rigidBody), mesh(mesh), isTemplate(true) {}
    ~Beyblade();

    void render(ShaderProgram& shader);

    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string setName() const { return name; }
    BeybladeBody* getRigidBody() const { return rigidBody; }
    BeybladeMesh* getMesh() const { return mesh; }
    void setName(const std::string& name) { Beyblade::name = name; }

    void setTemplateIndices(int layer, int disc, int driver) {
        if (!isTemplate) throw std::runtime_error("Cannot set template indices on a custom Beyblade!");
        templateIndices[0] = layer;
        templateIndices[1] = disc;
        templateIndices[2] = driver;
    }

    bool isTemplate = false;
    int templateIndices[3] = { -1, -1, -1 };  // ONLY used by templated beyblades

protected:

private:
    const int id; // Globally unique, will be managed by centralized server
    std::string name;
    BeybladeBody* rigidBody;
    BeybladeMesh* mesh;
};
