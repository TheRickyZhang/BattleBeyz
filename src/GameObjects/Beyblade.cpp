////////////////////////////////////////////////////////////////////////////////
// Beyblade.cpp -- Beyblade object code -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "Beyblade.h"

/**
* Constructor.
*/

Beyblade::Beyblade(BeybladeBody* rigidBody, BeybladeMesh* mesh, std::string name)
        : rigidBody(rigidBody), mesh(mesh), name(name)
{
    // NEWMESH: NOW DONE IN MAIN CODE:
    // mesh->initializeMesh();
}

/**
* Destructor.
*/

Beyblade::~Beyblade() {
    delete rigidBody;
    delete mesh;
}

void Beyblade::render(ShaderProgram& shader)
{
    shader.use();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), rigidBody->getCenter());

    shader.setUniformMat4("model", model);

    shader.setUniformVec3("lightColor", glm::vec3(1.0f));
    shader.setUniformVec3("lightPos", glm::vec3(0.0f, 1e6f, 0.0f));

    for (const auto& material : mesh->getMaterialColors()) {
        //        std::cout << "Setting material color: " << material.second.x << ", " << material.second.y << ", " << material.second.z << std::endl;
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