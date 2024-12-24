#include "BeybladeMesh.h"

// Might need to only define in cpp file?
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "MessageLog.h"

using namespace std;
/**
* NEWMESH Load the model files.
*
* A model consists of a Wavefront .obj model and a corresponding .mtl (material) mtl file.
*
* The model will always be centered and the based moved to a y-offset of 0.
*
* If the model consists of exactly three parts we calculate the radius and height of
* each part, assuming that the input order is disc, layer, and driver.
*
* If there are not three parts, the radii and heights are estimated, and from a physics
* standpoint you will be dealing with a cylinder.
*
* This does not do any scaling.  The expected sizes are documented in Units.txt.
*
* Once the model is loaded, the radii and heights can be copied to the Beyblade object
* so they can be more conveniently be accessed by the physics code.
*
* @param path                       [in] Path to the obj file.
*
* @return true on success.  Also sets the modelLoaded field.
*/

bool BeybladeMesh::loadModel(const string& path) {
    // Load the OBJ file...
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    string warn, err;

    modelLoaded = false;

    // Extract directory from the path
    string baseDir = path.substr(0, path.find_last_of("/\\"));
    if (baseDir.empty()) {
        baseDir = ".";
    }

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), baseDir.c_str(), true, true);

    if (!warn.empty()) {
        cout << "WARN::TINYOBJLOADER::" << warn << endl;
    }

    if (!err.empty()) {
        cerr << "ERROR::TINYOBJLOADER::" << err << endl;
    }

    if (!ret) {
        cerr << "ERROR::TINYOBJLOADER::Failed to load/parse .obj.\n";
        return false;
    }

    if (shapes.size() == 0) {
        cerr << "ERROR: No shapes found in " << path << endl;
        return false;
    }

    // Clear previous data
    vertices.clear();
    normals.clear();
    texCoords.clear();
    colors.clear();
    indices.clear();
    materialColors.clear();

    unordered_map<uint32_t, glm::vec3> vertexMap;
    unordered_map<uint32_t, glm::vec3> normalMap;
    unordered_map<uint32_t, glm::vec2> texCoordMap;

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

    unordered_map<uint32_t, glm::vec3> materialIndexToDiffuseColor;

    for (uint32_t i = 0; i < (uint32_t)materials.size(); ++i) {
        const auto& material = materials[i];
        glm::vec3 diffuseColor(material.diffuse[0], material.diffuse[1], material.diffuse[2]);

        // Store the material properties in the map

        materialColors[material.name] = diffuseColor;
        materialIndexToDiffuseColor[i] = diffuseColor;

        //cout << "Material name: " << material.name << endl;
        //cout << "Diffuse: " << diffuseColor.x << ", " << diffuseColor.y << ", " << diffuseColor.z << endl;
    }

    // Extract indices and assemble vertex data

    vector<BoundingBox> shapeBounds;  // min and max bounds for each shape

    for (const auto& shape : shapes) {
        BoundingBox bb(glm::vec3(1e6), glm::vec3(FLT_MIN));

        for (size_t faceIndex = 0; faceIndex < shape.mesh.indices.size() / 3; ++faceIndex) {
            int materialIndex = shape.mesh.material_ids[faceIndex];
            glm::vec3 color = materialIndexToDiffuseColor.count(materialIndex) ? materialIndexToDiffuseColor[materialIndex] : glm::vec3(1.0f, 1.0f, 1.0f);

            for (size_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex) {
                auto& index = shape.mesh.indices[3 * faceIndex + vertexIndex];
                glm::vec3 vertex = vertexMap[index.vertex_index];
                glm::vec3 normal = normalMap.count(index.normal_index) ? normalMap[index.normal_index] : glm::vec3(0.0f, 0.0f, 0.0f);
                glm::vec2 texCoord = texCoordMap.count(index.texcoord_index) ? texCoordMap[index.texcoord_index] : glm::vec2(0.0f, 0.0f);

                // Update bounding box for overall mesh.

                if (vertex.x < boundingBox.min.x) boundingBox.min.x = vertex.x;
                if (vertex.y < boundingBox.min.y) boundingBox.min.y = vertex.y;
                if (vertex.z < boundingBox.min.z) boundingBox.min.z = vertex.z;

                if (vertex.x > boundingBox.max.x) boundingBox.max.x = vertex.x;
                if (vertex.y > boundingBox.max.y) boundingBox.max.y = vertex.y;
                if (vertex.z > boundingBox.max.z) boundingBox.max.z = vertex.z;

                // Update bounding box for this subpart.

                if (vertex.x < bb.min.x) bb.min.x = vertex.x;
                if (vertex.y < bb.min.y) bb.min.y = vertex.y;
                if (vertex.z < bb.min.z) bb.min.z = vertex.z;

                if (vertex.x > bb.max.x) bb.max.x = vertex.x;
                if (vertex.y > bb.max.y) bb.max.y = vertex.y;
                if (vertex.z > bb.max.z) bb.max.z = vertex.z;

                vertices.push_back(vertex);
                normals.push_back(normal);
                texCoords.push_back(texCoord);
                indices.push_back((uint32_t)indices.size());
                colors.push_back(color);
            }
        }
        shapeBounds.push_back(bb);
    }

    // Center the mesh in the x and z planes, and move vertically so the base is at zero.

    float centerX = (boundingBox.min.x + boundingBox.max.x) * 0.5f;
    float centerZ = (boundingBox.min.z + boundingBox.max.z) * 0.5f;

    for (auto& v : vertices) {
        v.x -= centerX;
        v.y -= boundingBox.min.y;
        v.z -= centerZ;

    }
    boundingBox.max.y -= boundingBox.min.y;
    boundingBox.min.y = 0.0f;

    // If we have three parts, we want to know the radii for the disc, layer, and driver components.
    // These different radii are used for the various physics calculations such as air resistance, etc.
    //
    // Otherwise, use the maximum radius for each component.  In this case, the treatment in the
    // physics code will be the same as if the blade is a cylinder.

    if (shapes.size() != 3) {
        radiusDisc = radiusDriver = radiusLayer = (boundingBox.max.x - boundingBox.min.x) * 0.5f;
        heightDisc = heightDriver = heightLayer = (boundingBox.max.y - boundingBox.min.y) / 3.0f;  // Of course min should be 0 by now
    }
    else {
        radiusDisc = (shapeBounds[0].max.x - shapeBounds[0].min.x) * 0.5f;
        heightDisc = shapeBounds[0].max.y - shapeBounds[0].min.y;
        radiusLayer = (shapeBounds[1].max.x - shapeBounds[1].min.x) * 0.5f;
        heightLayer = shapeBounds[1].max.y - shapeBounds[1].min.y;
        radiusDriver = (shapeBounds[2].max.x - shapeBounds[2].min.x) * 0.5f;
        heightDriver = shapeBounds[2].max.y - shapeBounds[2].min.y;
    }
    modelPath = path;

    std::ostringstream oss;
    oss << "Model bounds ("
        << boundingBox.min.x << ", " << boundingBox.min.y << ", " << boundingBox.min.z
        << ") to ("
        << boundingBox.max.x << ", " << boundingBox.max.y << ", " << boundingBox.max.z
        << ").\n";
    oss << "Model loaded successfully with " << vertices.size() << " vertices and " << indices.size() << " indices." << endl;
    std::string meshInitString = oss.str();
    MessageLog::getInstance().addMessage(meshInitString, MessageType::NORMAL);

    modelLoaded = true;
    return true;
}

/**
* Internal routine to initialize the mesh.
*/

void BeybladeMesh::initializeMesh() {
    loadModel(modelPath);

    if (vertices.size() != normals.size() || vertices.size() != texCoords.size()) {
        cerr << "Mesh data is inconsistent" << endl;
        cout << "Vertices: " << vertices.size() << ", Normals: " << normals.size() << ", TexCoords: "
            << texCoords.size() << endl;
        return;
    }

    // Ensure colors vector has the same size as vertices if colors are not present
    if (colors.empty()) {
        colors.resize(vertices.size(), glm::vec3(1.0f, 1.0f, 1.0f));  // Default color to white
        cout << "SET TO ALL WHITE" << endl;
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
        }
        else {
            vertexData.push_back(0.0f);
            vertexData.push_back(0.0f);
        }
        
        if (tint != glm::vec3(1.0)) {
            cout << "Applying tint of " << tint.x << " " << tint.y << " " << tint.z << endl;
            // TODO: Create a globally accessible shader instance or expose how this is being shaded
            //shader.setTint(glm::vec3(1.0f, 0.0f, 0.0f));
        }

        // Color data
        vertexData.push_back(colors[i].x);
        vertexData.push_back(colors[i].y);
        vertexData.push_back(colors[i].z);
    }
    setupBuffers(VAO, VBO, EBO, vertexData.data(), vertexData.size() * sizeof(float), indices.data(),
        indices.size() * sizeof(uint32_t), { 3, 3, 2, 3 });
}

void BeybladeMesh::printDebugInfo() {
    ostringstream buffer;
    buffer << "Vertices: " << vertices.size() << endl;
    for (const auto& vertex : vertices) {
        buffer << fixed << setprecision(2) << "(" << vertex.x << ", " << vertex.y << ", " << vertex.z << ") ";
    }
    buffer << "\nNormals: " << normals.size() << endl;
    for (const auto& normal : normals) {
        buffer << fixed << setprecision(2) << "(" << normal.x << ", " << normal.y << ", " << normal.z << ") ";
    }
    buffer << "\nTexture Coordinates: " << texCoords.size() << endl;
    for (const auto& texCoord : texCoords) {
        buffer << fixed << setprecision(2) << "(" << texCoord.x << ", " << texCoord.y << ") ";
    }
    buffer << "\nIndices: " << indices.size() << endl;
    for (size_t i = 0; i < indices.size(); i += 3) {
        buffer << "Triangle: (" << indices[i] << ", " << indices[i + 1] << ", " << indices[i + 2] << ") ";
    }
    buffer << "\nTangents: " << tangents.size() << endl;
    for (const auto& tangent : tangents) {
        buffer << fixed << setprecision(2) << "(" << tangent.x << ", " << tangent.y << ", " << tangent.z << ") ";
    }
    cout << buffer.str();
}


void BeybladeMesh::render(ShaderProgram& shader) {
    // TODO: First of all, not correct logic, should be done per vertex, not per the entire mesh
    // Secondly, cannot set vertexColor? I am still a bit shaky on what exactly everything represents in the vertex and fragment shaders.
    //for (const auto& material : materialColors) {
    //    shader.setVec3("color", material.second);
    //}
    shader.setTint(tint);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Switch to wireframe mode

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Switch to polygon mode

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cerr << "OpenGL error: " << err << endl;
    }
}