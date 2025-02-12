////////////////////////////////////////////////////////////////////////////////
// Beyblade.h -- Beyblade object include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <stdexcept>

#include "json.hpp"

#include "BeybladeMesh.h"
#include "BeybladeBody.h"
#include "BeybladeTemplate.h"

class ObjectShader;

class Beyblade {
    friend class BeybladeMesh;
    friend class BeybladeBody;
public:
    // Note this beyblade id is GLOBALLY UNIQUE
    Beyblade(int id, const std::string& name, bool isTemplate = false);

    static Beyblade fromJson(const nlohmann::json& j);

    void render(ObjectShader& shader);

    int getId() const;
    std::string getName() const;
    void setName(const std::string &newName);

    BeybladeBody *getBody();
    BeybladeMesh *getMesh();
    void setMesh(std::unique_ptr<BeybladeMesh> &newMesh);

    void update(int layerIndex, int discIndex, int driverIndex);

    nlohmann::json toJson() const;

    bool isTemplate = false;
    int templateIndices[3] = { -1, -1, -1 };  // ONLY used by templated beyblades


protected:

private:
    const int id; // Globally unique, will be managed by centralized server
    std::string name;
    std::unique_ptr<BeybladeBody> body;
    std::unique_ptr<BeybladeMesh> mesh;

    void setTemplateIndices(int layerIndex, int discIndex, int driverIndex);
};