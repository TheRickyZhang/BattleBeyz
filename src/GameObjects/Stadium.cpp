////////////////////////////////////////////////////////////////////////////////
// Stadium.cpp -- Stadium Code -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include <glm/gtc/matrix_transform.hpp>
#include "Buffers.h"
#include "Stadium.h"
#include "ObjectShader.h"

using namespace std;

Stadium::Stadium(
    const std::string name,
    const glm::vec3& center,
    float radius,
    float curvature,
    float coefficientOfFriction,
    int verticesPerRing,
    int numRings,
    const glm::vec3& ringColor,
    const glm::vec3& crossColor,
    const glm::vec3& tint,
    std::shared_ptr<Texture> texture,
    float textureScale
)
    : name(std::move(name)),
    center(Vec3_M(center)),
    radius(M(radius)),
    curvature(Scalar(curvature)),
    scaledCurvature(__M(curvature / radius)),
    coefficientOfFriction(Scalar(coefficientOfFriction)),
    verticesPerRing(verticesPerRing),
    numRings(numRings),
    ringColor(ringColor),
    crossColor(crossColor),
    texture(std::move(texture)),
    textureScale(textureScale)
{
    setTint(tint);
    initializeMesh();
}


/**
* Stadium renderer
*/

void Stadium::render(ObjectShader& shader) {
    setModelMatrix(glm::translate(glm::mat4(1.0f), center.value())); // TODO: This only needs to be called when the center is set/changed

    // Assume that stadium textures are tied to the object itself
    MeshObject::render(shader, texture.get());
}

/**
* Checks whether a point is in the x-z plane of the stadium.
*/
bool Stadium::isInside(M x, M z) const {
    M scaledX = x - center.xTyped();
    M scaledZ = z - center.zTyped();
    return scaledX * scaledX + scaledZ * scaledZ < radius * radius;
}

// Returns the y-coordinate of the stadium at a given r in LOCAL space, where the stadium bottom (vertex) is at 0.0
const M Stadium::getYLocal(M r) const
{
    return scaledCurvature * r * r;
}

/**
* Returns the y-coordinate of the stadium at a given x and z.
*/
const M Stadium::getY(M x, M z) const {
    M scaledX = x - center.xTyped();
    M scaledZ = z - center.zTyped();
    M scaledY = scaledCurvature * (scaledX * scaledX + scaledZ * scaledZ);
    return scaledY + center.yTyped();
}

/**
* Returns the unit normal of the stadium at a given x and z.
*/
const Vec3_Scalar Stadium::getNormal(M x, M z) const {
    M scaledX = x - center.xTyped();
    M scaledZ = z - center.zTyped();
    Vec3_Scalar normal = normalize(Vec3_Scalar(
        (-2.0__ * scaledCurvature * scaledX).value(),
        1.0f,
        (-2.0__ * scaledCurvature * scaledZ).value()));
    return normal;
}



void Stadium::initializeMesh() {
    std::vector<float> vertexData;

    // Clear existing data
    vertices.clear();
    normals.clear();
    texCoords.clear();
    indices.clear();
    //tangents.clear();

    // Add center vertex first
    vertices.emplace_back(0, 0, 0);
    texCoords.emplace_back(0.5, 0.5);
    colors.emplace_back(crossColor);

    if (verticesPerRing % 4 != 0) {
        std::cerr << "Vertices per ring must be a multiple of 4" << std::endl;
        return;
    }

    float radius = this->radius.value();

    // Generate vertices
    for (int rIdx = 1; rIdx <= numRings; ++rIdx) {
        float r = powf(static_cast<float>(rIdx) / numRings, 0.5f) * static_cast<float>(radius);
        for (int thetaIdx = 0; thetaIdx < verticesPerRing; ++thetaIdx) {
            float theta = (float)(2.0f * M_PI * static_cast<float>(thetaIdx) / static_cast<float>(verticesPerRing));
            float x = r * std::cos(theta);
            float z = r * std::sin(theta);
            float y = getYLocal(M(r)).value();

            vertices.emplace_back(x, y, z);
            texCoords.emplace_back(textureScale * (r / radius * std::cos(theta)) + 0.5f,
                textureScale * (r / radius * std::sin(theta)) + 0.5f);

            // Set ring color for middle and end
            if (rIdx == numRings / 4 || rIdx == numRings - 1) {
                colors.emplace_back(ringColor);
            }
            else if (thetaIdx == 0 || thetaIdx == verticesPerRing / 4 || thetaIdx == verticesPerRing / 2 || thetaIdx == 3 * verticesPerRing / 4) {
                colors.emplace_back(crossColor);
            }
            else {
                colors.emplace_back(tint);
            }
            //            // Create a spiral color pattern
            //            if ((rIdx + thetaIdx * 2) % (verticesPerRing / 2) < verticesPerRing / (numRings / 2)) {
            //                colors.emplace_back(ringColor);
            //            } else if (thetaIdx % (verticesPerRing / 12) == 0) {
            //                colors.emplace_back(crossColor);
            //            } else {
            //                colors.emplace_back(color);
            //            }
        }
    }

    normals.resize(vertices.size(), glm::vec3(0.0f));
    //tangents.resize(vertices.size(), glm::vec3(0.0f));

    // Draw from origin to first ring
    for (int i = 0; i < verticesPerRing; ++i) {
        int origin = 0;
        int curr = i + 1;
        int next = (i + 1) % verticesPerRing + 1;
        indices.push_back(origin);
        indices.push_back(curr);
        indices.push_back(next);
        glm::vec3 v0 = vertices[origin];
        glm::vec3 v1 = vertices[curr];
        glm::vec3 v2 = vertices[next];
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        normals[origin] += normal;
        normals[curr] += normal;
        normals[next] += normal;
        // Calculate tangent for the triangle
        glm::vec2 uv0 = texCoords[origin];
        glm::vec2 uv1 = texCoords[curr];
        glm::vec2 uv2 = texCoords[next];
        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;
        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        //tangents[origin] += tangent;
        //tangents[curr] += tangent;
        //tangents[next] += tangent;
    }

    // Do pairs starting from (1, 2) to (n-1, n)
    for (int rIdx = 1; rIdx < numRings; ++rIdx) {
        int offset1 = (rIdx - 1) * verticesPerRing + 1;
        int offset2 = rIdx * verticesPerRing + 1;

        for (int vertIdx = 0; vertIdx < verticesPerRing; ++vertIdx) {
            int curr1 = offset1 + vertIdx;
            int next1 = offset1 + (vertIdx + 1) % verticesPerRing;
            int curr2 = offset2 + vertIdx;
            int next2 = offset2 + (vertIdx + 1) % verticesPerRing;

            // Triangle 1: (curr1, curr2, next1)
            indices.push_back(curr1);
            indices.push_back(curr2);
            indices.push_back(next1);

            // Calculate normal for Triangle 1
            glm::vec3 v0 = vertices[curr1];
            glm::vec3 v1 = vertices[curr2];
            glm::vec3 v2 = vertices[next1];
            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

            normals[curr1] += normal;
            normals[curr2] += normal;
            normals[next1] += normal;

            // Calculate tangent for Triangle 1
            glm::vec2 uv0 = texCoords[curr1];
            glm::vec2 uv1 = texCoords[curr2];
            glm::vec2 uv2 = texCoords[next1];
            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;
            //float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            //glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
            //tangents[curr1] += tangent;
            //tangents[curr2] += tangent;
            //tangents[next1] += tangent;

            // Triangle 2: (next1, curr2, next2)
            indices.push_back(next1);
            indices.push_back(curr2);
            indices.push_back(next2);

            // Calculate normal for Triangle 2
            v0 = vertices[next1];
            v1 = vertices[curr2];
            v2 = vertices[next2];
            edge1 = v1 - v0;
            edge2 = v2 - v0;
            normal = glm::normalize(glm::cross(edge1, edge2));

            normals[next1] += normal;
            normals[curr2] += normal;
            normals[next2] += normal;

            // Calculate tangent for Triangle 2
            uv0 = texCoords[next1];
            uv1 = texCoords[curr2];
            uv2 = texCoords[next2];
            deltaUV1 = uv1 - uv0;
            deltaUV2 = uv2 - uv0;
            //f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            //tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
            //tangents[next1] += tangent;
            //tangents[curr2] += tangent;
            //tangents[next2] += tangent;
        }
    }

    for (auto& normal : normals) {
        normal = glm::normalize(normal);
    }
    //for (auto& tangent : tangents) {
    //    tangent = glm::normalize(tangent);
    //}

    // Calculate bounding boxes for each triangle
    for (size_t i = 0; i < indices.size(); i += 3) {
        glm::vec3 v1 = vertices[indices[i]];
        glm::vec3 v2 = vertices[indices[i + 1]];
        glm::vec3 v3 = vertices[indices[i + 2]];

        auto bbox = new BoundingBox();
        bbox->update(v1, v2, v3);

        // Add the bounding box to the immovable rigid body
        boundingBoxes.push_back(bbox);
    }

    /*******************************************************************/

    if (vertices.size() != normals.size() || vertices.size() != texCoords.size()) {
        std::cerr << "Mesh data is inconsistent" << std::endl;
        std::cout << "Vertices: " << vertices.size() << ", Normals: " << normals.size() << ", TexCoords: " << texCoords.size() << std::endl;
        return;
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

        // Texture coordinates
        vertexData.push_back(texCoords[i].x);
        vertexData.push_back(texCoords[i].y);

        // Color data
        vertexData.push_back(colors[i].x);
        vertexData.push_back(colors[i].y);
        vertexData.push_back(colors[i].z);
    }
    setupBuffers(VAO, VBO, EBO, vertexData.data(), vertexData.size() * sizeof(float), indices.data(),
        indices.size() * sizeof(unsigned int), { 3, 3, 2, 3 });
}