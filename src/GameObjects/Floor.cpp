#include "Floor.h"
#include <glm/gtc/matrix_transform.hpp>

Floor::Floor(float w, float d, float y, float centerX, float centerZ)
    : MeshObject(), center(centerX, y, centerZ) {
    updateMesh();

    // Precompute the model matrix for the floor based on center
    setModelMatrix(glm::translate(glm::mat4(1.0f), center) *
        glm::scale(glm::mat4(1.0f), glm::vec3(w, 1.0f, d)));
}

void Floor::updateMesh() {
    // Define vertices for a unit quad centered at the origin
    vertices = {
        {-0.5f, 0.0f, -0.5f},
        { 0.5f, 0.0f, -0.5f},
        { 0.5f, 0.0f,  0.5f},
        {-0.5f, 0.0f,  0.5f}
    };

    // Define normals pointing up
    normals = {
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    };

    // Define texture coordinates
    texCoords = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}
    };
    //texCoords = {
    //    {0.0f, 0.0f},
    //    {textureScale.x, 0.0f},
    //    {textureScale.x, textureScale.y},
    //    {0.0f, textureScale.y}
    //};

    // **Define colors for each vertex**
    colors = {
        {0.5f, 0.5f, 0.5f}, // Color for vertex 0
        {0.5f, 0.5f, 0.5f}, // Color for vertex 1
        {0.5f, 0.5f, 0.5f}, // Color for vertex 2
        {0.5f, 0.5f, 0.5f}  // Color for vertex 3
    };

    // Define indices for two triangles
    indices = { 0, 1, 2, 2, 3, 0 };

    // Set up the buffers with the updated vertex data
    setupBuffersFromMembers();
}
