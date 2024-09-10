////////////////////////////////////////////////////////////////////////////////
// Beyblade.cpp -- Beyblade object code -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "Beyblade.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

/**
* Constructor.
*/

Beyblade::Beyblade(std::string modelPath, uint32_t vao, uint32_t vbo, uint32_t ebo,
                   BeybladeBody* rigidBody, std::string name)
        : modelPath(std::move(modelPath)), GameObject(vao, vbo, ebo, glm::vec3(1.0)), rigidBody(rigidBody), name(name) {
    Beyblade::initializeMesh();
}

/**
* Destructor.
*/

Beyblade::~Beyblade() {
    delete texture;
}

/**
* Internal routine to initialize the mesh.
*/

void Beyblade::initializeMesh() {
    loadModel(modelPath);

    if (vertices.size() != normals.size() || vertices.size() != texCoords.size()) {
        std::cerr << "Mesh data is inconsistent" << std::endl;
        std::cout << "Vertices: " << vertices.size() << ", Normals: " << normals.size() << ", TexCoords: "
                  << texCoords.size() << std::endl;
        return;
    }

    // Ensure colors vector has the same size as vertices if colors are not present
    if (colors.empty()) {
        colors.resize(vertices.size(), glm::vec3(1.0f, 1.0f, 1.0f));  // Default color to white
        std::cout << "SET TO ALL WHITE" << std::endl;
    }

    for (size_t i = 0; i < vertices.size(); ++i) {
        // Vertex positions
        vertexData.push_back(vertices[i].x);
        vertexData.push_back(vertices[i].y);
        vertexData.push_back(vertices[i].z);

        // Normal data
        vertexData.push_back(normals[i].x);
        vertexData.push_back(normals[i].y);
        vertexData.push_back(normals[i].z);

        // Texture coordinates (if available)
        if (!texCoords.empty()) {
            vertexData.push_back(texCoords[i].x);
            vertexData.push_back(texCoords[i].y);
        } else {
            vertexData.push_back(0.0f);  // Default texture coordinate
            vertexData.push_back(0.0f);
        }

        // Color data
        vertexData.push_back(colors[i].x);
        vertexData.push_back(colors[i].y);
        vertexData.push_back(colors[i].z);
    }

    setupBuffers(VAO, VBO, EBO, vertexData.data(), vertexData.size() * sizeof(float), indices.data(),
                 indices.size() * sizeof(uint32_t));
}

/**
* Load the model files.
* 
* A model consists of a Wavefront .obj model and a corresponding .mtl (material) mtl file.
*/

void Beyblade::loadModel(const std::string& path) {
    // Load the OBJ file...
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Extract directory from the path
    std::string baseDir = path.substr(0, path.find_last_of("/\\"));
    if (baseDir.empty()) {
        baseDir = ".";
    }

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), baseDir.c_str(), true, true);

    if (!warn.empty()) {
        std::cout << "WARN::TINYOBJLOADER::" << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "ERROR::TINYOBJLOADER::" << err << std::endl;
    }

    if (!ret) {
        std::cerr << "ERROR::TINYOBJLOADER::Failed to load/parse .obj.\n";
        return;
    }

    // Clear previous data
    vertices.clear();
    normals.clear();
    texCoords.clear();
    colors.clear();
    indices.clear();
    materialColors.clear();

    std::unordered_map<uint32_t, glm::vec3> vertexMap;
    std::unordered_map<uint32_t, glm::vec3> normalMap;
    std::unordered_map<uint32_t, glm::vec2> texCoordMap;

    // Extract vertices, normals, and texture coordinates...
    for (uint32_t v = 0; v < (uint32_t)attrib.vertices.size() / 3; ++v) {
        vertexMap[v] = glm::vec3(attrib.vertices[3 * v + 0], attrib.vertices[3 * v + 1], attrib.vertices[3 * v + 2]);
    }

    for (uint32_t n = 0; n < (uint32_t)attrib.normals.size() / 3; ++n) {
        normalMap[n] = glm::vec3(attrib.normals[3 * n + 0], attrib.normals[3 * n + 1], attrib.normals[3 * n + 2]);
    }

    for (uint32_t t = 0; t < (uint32_t)attrib.texcoords.size() / 2; ++t) {
        texCoordMap[t] = glm::vec2(attrib.texcoords[2 * t + 0], attrib.texcoords[2 * t + 1]);
    }

    // Handle materials

    std::unordered_map<uint32_t, glm::vec3> materialIndexToDiffuseColor;

    for (uint32_t i = 0; i < (uint32_t)materials.size(); ++i) {
        const auto& material = materials[i];
        glm::vec3 diffuseColor(material.diffuse[0], material.diffuse[1], material.diffuse[2]);

        // Store the material properties in the map

        materialColors[material.name] = diffuseColor;
        materialIndexToDiffuseColor[i] = diffuseColor;

        std::cout << "Material name: " << material.name << std::endl;
        std::cout << "Diffuse: " << diffuseColor.x << ", " << diffuseColor.y << ", " << diffuseColor.z << std::endl;
    }

    // Extract indices and assemble vertex data

    for (const auto& shape : shapes) {
        for (size_t faceIndex = 0; faceIndex < shape.mesh.indices.size() / 3; ++faceIndex) {
            int materialIndex = shape.mesh.material_ids[faceIndex];
            glm::vec3 color = materialIndexToDiffuseColor.count(materialIndex) ? materialIndexToDiffuseColor[materialIndex] : glm::vec3(1.0f, 1.0f, 1.0f);

            for (size_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex) {
                auto& index = shape.mesh.indices[3 * faceIndex + vertexIndex];
                glm::vec3 vertex = vertexMap[index.vertex_index];
                glm::vec3 normal = normalMap.count(index.normal_index) ? normalMap[index.normal_index] : glm::vec3(0.0f, 0.0f, 0.0f);
                glm::vec2 texCoord = texCoordMap.count(index.texcoord_index) ? texCoordMap[index.texcoord_index] : glm::vec2(0.0f, 0.0f);

                vertices.push_back(vertex);
                normals.push_back(normal);
                texCoords.push_back(texCoord);
                indices.push_back((uint32_t)indices.size());
                colors.push_back(color);
            }
        }
    }

    std::cout << "Model loaded successfully with " << vertices.size() << " vertices and " << indices.size() << " indices." << std::endl;
}

/**
* Print some structural info for debugging.
*/

void Beyblade::printDebugInfo() {
    std::ostringstream buffer;
    buffer << "Vertices: " << vertices.size() << std::endl;
    for (const auto& vertex : vertices) {
        buffer << std::fixed << std::setprecision(2) << "(" << vertex.x << ", " << vertex.y << ", " << vertex.z << ") ";
    }
    buffer << "\nNormals: " << normals.size() << std::endl;
    for (const auto& normal : normals) {
        buffer << std::fixed << std::setprecision(2) << "(" << normal.x << ", " << normal.y << ", " << normal.z << ") ";
    }
    buffer << "\nTexture Coordinates: " << texCoords.size() << std::endl;
    for (const auto& texCoord : texCoords) {
        buffer << std::fixed << std::setprecision(2) << "(" << texCoord.x << ", " << texCoord.y << ") ";
    }
    buffer << "\nIndices: " << indices.size() << std::endl;
    for (size_t i = 0; i < indices.size(); i += 3) {
        buffer << "Triangle: (" << indices[i] << ", " << indices[i + 1] << ", " << indices[i + 2] << ") ";
    }
    buffer << "\nTangents: " << tangents.size() << std::endl;
    for (const auto& tangent : tangents) {
        buffer << std::fixed << std::setprecision(2) << "(" << tangent.x << ", " << tangent.y << ", " << tangent.z << ") ";
    }
    std::cout << buffer.str();
}

/**
* Renderer.
* 
* @param shader                     [in] Our custom shader program.
* 
* @param lightColor                 [in] The light color.
* 
* @param lightPos                   [in] The light position.
*/

void Beyblade::render(ShaderProgram& shader, const glm::vec3& lightColor, const glm::vec3& lightPos)
{
    shader.use();

    if (texture) {
        glActiveTexture(GL_TEXTURE0);
        texture->use();
        shader.setInt("texture1", 0);
    }

    glm::mat4 model = glm::translate(glm::mat4(1.0f), rigidBody->getCenter());
    shader.setUniformMat4("model", model);
    shader.setUniformVec3("lightColor", lightColor);
    shader.setUniformVec3("lightPos", lightPos);
    for(const auto& material : materialColors) {
//        std::cout << "Setting material color: " << material.second.x << ", " << material.second.y << ", " << material.second.z << std::endl;
        shader.setUniformVec3("VertexColor", material.second);
    }

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Switch to wireframe mode

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Switch to polygon mode

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}
