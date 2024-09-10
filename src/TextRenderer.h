////////////////////////////////////////////////////////////////////////////////
// TextRenderer.h -- Text Rendering object include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "ShaderProgram.h"
#include "Utils.h"
#include "ShaderPath.h"

class TextRenderer {
public:
    // Needs a path to .ttf font file, and VAO and VBO for rendering text
    TextRenderer(const char* fontPath, unsigned int VAO, unsigned int VBO);
    ~TextRenderer();

    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);
    ShaderProgram* getShaderProgram();

    void Resize(int width, int height);

private:
    FT_Library ft{};   // Must load FreeType library
    FT_Face face{};    // Contains font data/glyphs
    GLuint textureID{};
    GLuint VAO, VBO;

    // Shader program for rendering text: contains the vertex and fragment shaders
    ShaderProgram* shaderProgram{};

    void initRenderData();
};
