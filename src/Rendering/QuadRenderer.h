////////////////////////////////////////////////////////////////////////////////
// QuadRender.h -- QuadRender include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <GL/glew.h>

class ShaderProgram;

// ONLY use for drawing screens with backgroundShader
class QuadRenderer {
public:
    explicit QuadRenderer(float vecScale = 1.0f);
    ~QuadRenderer();
    void render(ShaderProgram& shader) const;

private:
    GLuint quadVAO, quadVBO, quadEBO;
};