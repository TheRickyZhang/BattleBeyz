////////////////////////////////////////////////////////////////////////////////
// Stadium.cpp -- Stadium Code inclde -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <vector>

#include "StadiumBody.h"
#include "StadiumMesh.h"

// Stadium with parabolic shape
class Stadium {
public:
    Stadium::Stadium(std::unique_ptr<StadiumBody> stadiumBody, std::unique_ptr<StadiumMesh> stadiumMesh, std::string name) :
        rigidBody(std::move(stadiumBody)), mesh(std::move(stadiumMesh)), name(std::move(name)) {
        std::cout << "In Stadium constructor:" << std::endl;
        std::cout << "  mesh pointer before initializeMesh: "
            << (mesh ? "not null" : "null") << std::endl;
        initializeMesh();
        std::cout << "  mesh pointer after initializeMesh: "
            << (mesh ? "not null" : "null") << std::endl;
    }

    void initializeMesh();

    void render(ObjectShader& shader);
    StadiumBody* getRigidBody() const { return rigidBody.get(); }
    StadiumMesh* getMesh() const { return mesh.get(); }

private:
    std::unique_ptr<StadiumBody> rigidBody;
    std::unique_ptr<StadiumMesh> mesh;
    std::string name;

    std::vector<std::unique_ptr<BoundingBox>> triangleBoundingBoxes;
};