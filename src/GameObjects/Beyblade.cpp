////////////////////////////////////////////////////////////////////////////////
// Beyblade.cpp -- Beyblade object code -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include <glm/gtc/matrix_transform.hpp>

#include "Beyblade.h"
#include "BeybladeParts.h"
#include "Buffers.h"
#include "GameObject.h"
#include "MeshObjects/BeybladeMesh.h"
#include "RigidBody.h"
#include "RigidBodies/BeybladeBody.h"
#include "ShaderProgram.h"
#include "MessageLog.h"

#include "Texture.h"
#include "Utils.h"

using namespace std;
using nlohmann::json;


void Beyblade::setTemplateIndices(int layerIndex, int discIndex, int driverIndex) {
    if (!isTemplate) throw runtime_error("Cannot set template indices on a custom Beyblade!");
    templateIndices[0] = layerIndex;
    templateIndices[1] = discIndex;
    templateIndices[2] = driverIndex;
}

/**
* Constructor.
*/
Beyblade::Beyblade(int id, const string& name, bool isTemplate) :
    id(id), name(name), isTemplate(isTemplate) {
    if (isTemplate) {
        setTemplateIndices(0, 0, 0);
        rigidBody = make_unique<BeybladeBody>(templateLayers[0].part, templateDiscs[0].part, templateDrivers[0].part);
        // TODO: Implement a combineMesh function. Requires all obj files to be standardized, centered and aligned correctly. Then combine them
        mesh = make_unique<BeybladeMesh>();
        //mesh = combineMesh(templateLayers[0].modelPath, templateDiscs[0].modelPath, templateDrivers[0].modelPath);
    }
    else {
        rigidBody = make_unique<BeybladeBody>();
        mesh = make_unique<BeybladeMesh>();
    }
}

int Beyblade::getId() const {
    return id;
}
string Beyblade::getName() const {
    return name;
}
BeybladeBody* Beyblade::getRigidBody() {
    return rigidBody.get();
}
BeybladeMesh* Beyblade::getMesh() {
    return mesh.get();
}
void Beyblade::setMesh(std::unique_ptr<BeybladeMesh> &newMesh) {
    mesh = std::move(newMesh);
}
void Beyblade::setName(const std::string& newName) { 
    name = newName;
}


void Beyblade::render(ShaderProgram& shader)
{
    shader.use();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), rigidBody->getCenter().value());

    shader.setMat4("model", model);

    mesh->render(shader);
}

void Beyblade::update(int layerIndex, int discIndex, int driverIndex) {
    setTemplateIndices(layerIndex, discIndex, driverIndex);
    TemplateFormat<Layer> layer = templateLayers[layerIndex];
    TemplateFormat<Disc> disc = templateDiscs[discIndex];
    TemplateFormat<Driver> driver = templateDrivers[driverIndex];
    rigidBody = make_unique<BeybladeBody>(layer.part, disc.part, driver.part);
    //mesh = make_unique<BeybladeMesh>(combineMesh(templateLayers[0].modelPath, templateDiscs[0].modelPath, templateDrivers[0].modelPath));
}


// JSON
json Beyblade::toJson() const {
    json j;
    j["id"] = id;
    j["name"] = name;
    j["isTemplate"] = isTemplate;

    // Serialize BeybladeBody
    if (rigidBody) {
        json bodyJson;

        // Serialize Layer
        bodyJson["layer"] = {
            { "radius", rigidBody->layer.radius.value() },
            { "height", rigidBody->layer.height.value() },
            { "mass", rigidBody->layer.mass.value() },
            { "momentOfInertia", rigidBody->layer.momentOfInertia.value() },
            { "rotationalDragCoefficient", rigidBody->layer.rotationalDragCoefficient.value() },
            { "recoilDistribution", {
                { "mean", rigidBody->layer.recoilDistribution.getMean().value() },
                { "stddev", rigidBody->layer.recoilDistribution.getStdDev().value() }
            }},
            { "coefficientOfRestitution", rigidBody->layer.coefficientOfRestitution.value() }
        };

        // Serialize Disc
        bodyJson["disc"] = {
            { "radius", rigidBody->disc.radius.value() },
            { "height", rigidBody->disc.height.value() },
            { "mass", rigidBody->disc.mass.value() },
            { "momentOfInertia", rigidBody->disc.momentOfInertia.value() },
            { "rotationalDragCoefficient", rigidBody->disc.rotationalDragCoefficient.value() },
        };

        // Serialize Driver
        bodyJson["driver"] = {
            { "radius", rigidBody->driver.radius.value() },
            { "height", rigidBody->driver.height.value() },
            { "mass", rigidBody->driver.mass.value() },
            { "momentOfInertia", rigidBody->driver.momentOfInertia.value() },
            { "rotationalDragCoefficient", rigidBody->driver.rotationalDragCoefficient.value() },
            { "coefficientOfFriction", rigidBody->driver.coefficientOfFriction.value() }
        };

        j["body"] = bodyJson;
    }

    // Serialize BeybladeMesh
    if (mesh) {
        json meshJson;
        meshJson["modelPath"] = mesh->getModelPath();
        j["mesh"] = meshJson;
    }

    return j;
}

Beyblade Beyblade::fromJson(const nlohmann::json& j) {
    int id = j.at("id").get<int>();
    std::string name = j.at("name").get<std::string>();
    bool isTemplate = j.at("isTemplate").get<bool>();

    Beyblade beyblade(id, name, isTemplate);

    // Deserialize body
    if (j.contains("body")) {
        const auto& bodyJson = j.at("body");
        auto rigidBody = std::make_unique<BeybladeBody>();

        // Layer
        if (bodyJson.contains("layer")) {
            const auto& layerJson = bodyJson.at("layer");
            rigidBody->layer.radius = M(layerJson.at("radius").get<float>());
            rigidBody->layer.height = M(layerJson.at("height").get<float>());
            rigidBody->layer.mass = Kg(layerJson.at("mass").get<float>());
            rigidBody->layer.momentOfInertia = KgM2(layerJson.at("momentOfInertia").get<float>());
            const auto& recoilJson = layerJson.at("recoilDistribution");
            rigidBody->layer.recoilDistribution = RandomDistribution(
                Scalar(recoilJson.at("mean").get<float>()),
                Scalar(recoilJson.at("stddev").get<float>())
            );
            rigidBody->layer.coefficientOfRestitution = Scalar(layerJson.at("coefficientOfRestitution").get<float>());
        }

        // Disc
        if (bodyJson.contains("disc")) {
            const auto& discJson = bodyJson.at("disc");
            rigidBody->disc.radius = M(discJson.at("radius").get<float>());
            rigidBody->disc.height = M(discJson.at("height").get<float>());
            rigidBody->disc.mass = Kg(discJson.at("mass").get<float>());
            rigidBody->disc.momentOfInertia = KgM2(discJson.at("momentOfInertia").get<float>());
            rigidBody->disc.rotationalDragCoefficient = Scalar(discJson.at("rotationalDragCoefficient").get<float>());
        }

        // Driver
        if (bodyJson.contains("driver")) {
            const auto& driverJson = bodyJson.at("driver");
            rigidBody->driver.radius = M(driverJson.at("radius").get<float>());
            rigidBody->driver.height = M(driverJson.at("height").get<float>());
            rigidBody->driver.mass = Kg(driverJson.at("mass").get<float>());
            rigidBody->driver.momentOfInertia = KgM2(driverJson.at("momentOfInertia").get<float>());
            rigidBody->driver.rotationalDragCoefficient = Scalar(driverJson.at("rotationalDragCoefficient").get<float>());
            rigidBody->driver.coefficientOfFriction = Scalar(driverJson.at("coefficientOfFriction").get<float>());
        }

        beyblade.rigidBody = std::move(rigidBody);
    }

    // Deserialize mesh (init() upon instantiation shoul handle most)
    if (j.contains("mesh")) {
        std::string modelPath = j.at("mesh").at("modelPath").get<std::string>();
        beyblade.mesh = std::make_unique<BeybladeMesh>(modelPath, 0, 0, 0, glm::vec3(1.0f)); // TOLOOK: Inspect what is really needed do be stored in this call. Not tested.
    }

    return beyblade;
}
