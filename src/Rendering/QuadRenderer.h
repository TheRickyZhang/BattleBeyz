////////////////////////////////////////////////////////////////////////////////
// QuadRender.h -- QuadRender include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

class ShaderProgram;

/* 
 * Draws a rectangle to the screen
 * 
 * Default arguments with backgroundShader are for drawing as a background
 * 
 * Custom arguments allow for other orientations like floors, walls, etc.
 */
class QuadRenderer {
public:
    explicit QuadRenderer(const glm::mat4& model, const glm::mat4& projection);
    ~QuadRenderer();
    void render(ShaderProgram& shader) const;

    void setModelMatrix(const glm::mat4& model);
    void setProjectionMatrix(const glm::mat4& projection);

private:
    GLuint quadVAO, quadVBO, quadEBO;
    glm::mat4 modelMatrix;
    glm::mat4 projectionMatrix;
};
