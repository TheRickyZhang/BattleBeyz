////////////////////////////////////////////////////////////////////////////////
// ObjectShader.h -- ObjectShader include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include "ShaderProgram.h"

enum LightType {
    Directional = 0,
    Point = 1,
    // Future: Spotlight = 2
};


class ObjectShader : public ShaderProgram {
public:
    ObjectShader(const char* vertexPath, const char* fragmentPath);

    // Global parameters - set once per frame
    void setGlobalRenderParams(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos) const;

    // Object-specific parameters - set for each object
    void setObjectRenderParams(const glm::mat4& model, const glm::vec3& tint) const;

    // Lighting parameters - optional (set in shader by default)
    void setLight(LightType lightType, const glm::vec3& lightColor, const glm::vec3& lightPos) const;
};
