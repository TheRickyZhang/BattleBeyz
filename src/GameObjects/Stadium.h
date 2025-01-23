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
#include <json.hpp>

#include "DefaultValues.h"
#include "MeshObject.h"
#include "BoundingBox.h"
#include "Units.h"
#include "Texture.h"
#include "ShaderPath.h"

using namespace Units;

class Stadium : public MeshObject {
public:
    Stadium(
        int id = -1,
        const std::string name = StadiumDefaults::name,
        const glm::vec3& center = StadiumDefaults::center,
        float radius = StadiumDefaults::radius,
        float curvature = StadiumDefaults::curvature,
        float COF = StadiumDefaults::COF,
        int verticesPerRing = StadiumDefaults::verticesPerRing,
        int numRings = StadiumDefaults::numRings,
        const glm::vec3& ringColor = StadiumDefaults::ringColor,
        const glm::vec3& crossColor = StadiumDefaults::crossColor,
        const glm::vec3& tint = StadiumDefaults::tint,
        std::shared_ptr<Texture> texture = DefaultTexture(),
        float textureScale = StadiumDefaults::textureScale
    );
    Stadium& Stadium::operator=(const Stadium& other) {
        if (this != &other) {
            this->id = -1; // Set to temporary. MUST set ID for no global conflicts
            this->name = other.name;
            this->center = other.center;
            this->radius = other.radius;
            this->curvature = other.curvature;
            this->scaledCurvature = other.scaledCurvature;
            this->coefficientOfFriction = other.coefficientOfFriction;
            this->verticesPerRing = other.verticesPerRing;
            this->numRings = other.numRings;
            this->ringColor = other.ringColor;
            this->crossColor = other.crossColor;
            this->textureScale = other.textureScale;
            this->texture = other.texture;  // Copy with shared ownership
            this->meshChanged = other.meshChanged;
        }
        return *this;
    }

    nlohmann::json toJson() const;
    static Stadium fromJson(const nlohmann::json& j);

    virtual void updateMesh() override;

    void render(ObjectShader& shader);

    bool isInside(M x, M z) const;
    const M getYLocal(M r) const;
    const M getY(M x, M z) const;
    const Vec3_Scalar getNormal(M x, M z) const;

    // Getters (read-only access)
    int getId() const { return id; }
    std::string getName() const { return name; }
    Vec3_M getCenter() const { return center; }
    const M getRadius() const { return radius; }
    const Scalar getCurvature() const { return curvature; }
    Scalar getCOF() const { return coefficientOfFriction; }
    int getVerticesPerRing() const { return verticesPerRing; }
    int getNumRings() const { return numRings; }
    glm::vec3 getRingColor() const { return ringColor; }
    glm::vec3 getCrossColor() const { return crossColor; }
    float getTextureScale() const { return textureScale; }
    std::shared_ptr<Texture> getTexture() const { return texture; }


    // Setters
    void setName(const std::string& newName) {
        name = newName;
    }
    void setRadius(float newRadius) {
        radius = M(newRadius);
        scaledCurvature = __M(curvature.value() / newRadius);
        meshChanged = true;
    }
    void setCurvature(float newCurvature) {
        curvature = Scalar(newCurvature);
        scaledCurvature = __M(newCurvature / radius.value());
        meshChanged = true;
    }
    void setFriction(float newFriction) {
        coefficientOfFriction = Scalar(newFriction);
    }
    void setCenter(const glm::vec3& newCenter) {
        center = Vec3_M(newCenter);
        meshChanged = true;
    }
    void setVerticesPerRing(int newVerticesPerRing) {
        verticesPerRing = newVerticesPerRing;
    }
    void setNumRings(int newNumRings) {
        numRings = newNumRings;
        meshChanged = true;
    }
    void setRingColor(const glm::vec3& newRingColor) {
        ringColor = newRingColor;
        meshChanged = true;
    }
    void setCrossColor(const glm::vec3& newCrossColor) {
        crossColor = newCrossColor;
        meshChanged = true;
    }
    void setTextureScale(float newTextureScale) {
        textureScale = newTextureScale;
        meshChanged = true;
    }
    void setTexture(std::shared_ptr<Texture> newTexture) {
        texture = std::move(newTexture);
        meshChanged = true;
    }

    bool getModified() const {
        return modified;
    }
    void setModified(bool _modified = true) {
        modified = _modified;
    }

    std::vector<BoundingBox*> boundingBoxes;
private:
    // General
    std::string name;
    int id;       // -1 is a temporary id. Otherwise, all other ids are 

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

    bool meshChanged = true;    // Whether the mesh has changed. Used for rendering purposes
    bool modified = false;      // Whether any parameter has changed. Used for customization update purposes.
};
