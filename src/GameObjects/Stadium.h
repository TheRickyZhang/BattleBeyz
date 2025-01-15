////////////////////////////////////////////////////////////////////////////////
// Stadium.cpp -- Stadium Code inclde -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

/**
 * Because the mathematical parameters / rendering process of stadiums are intertwined, this class combines mesh and body
 *
 * center is where the stadium is located in the world, so local coordinates must be shifted correspondingly
 *
 * Ordinary value ranges (SI Units):
 * - radius: 0.15 to 1.5
 * - curvature: 0.0 (flat) to 1.0 (approximately 45 degrees), independent of radius
 * - scaledCurvature: curvature to be used in calculations, inversely proportional to radius
 * - coefficientOfFriction: 0.0 to 0.5
 */
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include <glm/glm.hpp>

#include "MeshObject.h"
#include "BoundingBox.h"
#include "Units.h"
#include "Texture.h"
#include "ShaderPath.h"

using namespace Units;


class Stadium : public MeshObject {
public:
    Stadium(
        const std::string name  = "Default Stadium",
        const glm::vec3& center = glm::vec3(0.0f, 0.0f, 0.0f),
        float radius            = 1.2f, 
        float curvature         = 0.10f, 
        float COF               = 0.35f,
        int verticesPerRing     = 64,
        int numRings            = 10,
        const glm::vec3& ringColor  = glm::vec3(1.0f, 0.0f, 0.0f),
        const glm::vec3& crossColor = glm::vec3(0.0f, 0.0f, 1.0f),
        const glm::vec3& tint       = glm::vec3(1.0f),
        std::shared_ptr<Texture> texture = std::make_shared<Texture>(MISSING_TEXTURE_PATH),
        float textureScale = 1.5f
    );

    virtual void initializeMesh() override;

    void render(ObjectShader& shader);

    bool isInside(M x, M z) const;
    const M getYLocal(M r) const;
    const M getY(M x, M z) const;
    const Vec3_Scalar getNormal(M x, M z) const;

    // Getters (read-only access)
    Vec3_M getCenter() const { return center; }
    const M getRadius() const { return radius; }
    const Scalar getCurvature() const { return curvature; }
    Scalar getCOF() const { return coefficientOfFriction; }

    // Setters
    void setRadius(float newRadius) {
        radius = M(newRadius);
        scaledCurvature = __M(curvature.value() / newRadius);
    }
    void setCurvature(float newCurvature) {
        curvature = Scalar(newCurvature);
        scaledCurvature = __M(newCurvature / radius.value());
    }
    void setFriction(float newFriction) {
        coefficientOfFriction = Scalar(newFriction);
    }

    std::string getName() const {
        return name;
    }

    std::vector<BoundingBox*> boundingBoxes;
private:
    // General
    std::string name;

    // Physical values
    Vec3_M center;
    M radius;
    Scalar curvature;
    __M scaledCurvature;
    Scalar coefficientOfFriction;

    // Rendering
    int verticesPerRing;
    int numRings;
    glm::vec3 ringColor;
    glm::vec3 crossColor;
    float textureScale;
    std::shared_ptr<Texture> texture;
};
