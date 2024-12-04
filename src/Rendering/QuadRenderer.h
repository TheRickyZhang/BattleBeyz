////////////////////////////////////////////////////////////////////////////////
// QuadRender.h -- QuadRender include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

# pragma once

#include <GL/glew.h>

class QuadRenderer {
public:
    QuadRenderer(float vecScale = 1.0f);
    ~QuadRenderer();
    void render() const;

private:
    unsigned int quadVAO, quadVBO;
};