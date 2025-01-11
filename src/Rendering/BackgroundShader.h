////////////////////////////////////////////////////////////////////////////////
// BackgroundShader.h -- BackgroundShader include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include "ShaderProgram.h"

class BackgroundShader : public ShaderProgram {
public:
    BackgroundShader(const char* vertexPath, const char* fragmentPath);

    // Global parameters - set once per frame
    void setBackgroundGlobalParams(const glm::mat4& projection, float wrapFactor, float time) const;

    // Object-specific parameters - set for each background object
    void setBackgroundObjectParams(const glm::mat4& model, GLuint textureID = 0) const;
};
