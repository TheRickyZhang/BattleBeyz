////////////////////////////////////////////////////////////////////////////////
// Stadium.cpp -- Stadium Code inclde -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <memory>

#include "GameObject.h"
#include "Texture.h"
#include "Utils.h"
#include "Buffers.h"
#include "BoundingBox.h"
#include "RigidBodies/StadiumBody.h"
#include "MeshObjects/StadiumMesh.h"

// Stadium with parabolic shape
class Stadium {
public:
    Stadium::Stadium(StadiumBody* stadiumBody, StadiumMesh* stadiumMesh, std::string name) :
        rigidBody(stadiumBody), mesh(stadiumMesh), name(std::move(name)) {
        initializeMesh();
    }

    void initializeMesh();

    void render(ShaderProgram& shader);
    StadiumBody* getRigidBody() const { return rigidBody; }
    StadiumMesh* getMesh() const { return mesh; }

private:
    StadiumBody* rigidBody;
    StadiumMesh* mesh;
    std::string name;

    std::vector<std::unique_ptr<BoundingBox>> triangleBoundingBoxes;
};