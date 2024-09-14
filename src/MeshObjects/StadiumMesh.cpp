#include "StadiumMesh.h"

void StadiumMesh::printDebugInfo() {
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