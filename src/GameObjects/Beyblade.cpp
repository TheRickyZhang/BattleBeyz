////////////////////////////////////////////////////////////////////////////////
// Beyblade.cpp -- Beyblade object code -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "Beyblade.h"
using namespace std;

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

void Beyblade::render(ShaderProgram& shader)
{
    shader.use();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), rigidBody->getCenter());

    shader.setUniformMat4("model", model);

    shader.setUniformVec3("lightColor", glm::vec3(1.0f));
    shader.setUniformVec3("lightPos", glm::vec3(0.0f, 1e6f, 0.0f));

    for (const auto& material : mesh->getMaterialColors()) {
        shader.setUniformVec3("VertexColor", material.second);
    }

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Switch to wireframe mode

    glBindVertexArray(mesh->getVAO());
    glDrawElements(GL_TRIANGLES, (GLsizei)mesh->getIndicesSize(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Switch to polygon mode

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}

void Beyblade::update(int layerIndex, int discIndex, int driverIndex) {
    setTemplateIndices(layerIndex, discIndex, driverIndex);
    TemplateFormat<Layer> layer = templateLayers[layerIndex];
    TemplateFormat<Disc> disc = templateDiscs[discIndex];
    TemplateFormat<Driver> driver = templateDrivers[driverIndex];
    rigidBody = make_unique<BeybladeBody>(layer.part, disc.part, driver.part);
    //mesh = make_unique<BeybladeMesh>(combineMesh(templateLayers[0].modelPath, templateDiscs[0].modelPath, templateDrivers[0].modelPath));
}
