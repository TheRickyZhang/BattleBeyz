////////////////////////////////////////////////////////////////////////////////
// QuadRenderer.h -- QuadRenderer header -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "BackgroundShader.h"
#include "Texture.h"

class QuadRenderer {
public:
    // Only needs a model matrix now. Projection is handled globally.
    explicit QuadRenderer(const glm::mat4& model);
    ~QuadRenderer();

    void render(BackgroundShader& shader, std::shared_ptr<Texture> texture) const;

    void setModelMatrix(const glm::mat4& model);

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    glm::mat4 modelMatrix;

    void setupBuffers();
};
