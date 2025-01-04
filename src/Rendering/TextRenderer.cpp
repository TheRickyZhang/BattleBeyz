////////////////////////////////////////////////////////////////////////////////
// TextRenderer.cpp -- Text Rendering object -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TextRenderer.h"
#include "ObjectShader.h"

//#include "Utils.h"
#include "ShaderPath.h"

/**
* Constructor.
*/

TextRenderer::TextRenderer(const char* fontPath, unsigned int VAO, unsigned int VBO)
        : VAO(VAO), VBO(VBO) {
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "Could not init FreeType Library" << std::endl;
        return;
    }

    if (FT_New_Face(ft, fontPath, 0, &face)) {
        std::cerr << "Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    // Boilerplate text initialization
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    initRenderData();
}

/**
* Destructor.
*/

TextRenderer::~TextRenderer() {
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    delete shaderProgram;  // Clean up the shader program
}

/**
* Get the current shader program.
*
* @return The shader program.
*/

ShaderProgram* TextRenderer::getShaderProgram() {
    return shaderProgram;
}

/**
* Initialization.
*/

void TextRenderer::initRenderData() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Allocate memory for 6 vertices with {x, y, z, w} coordinates. Dynamic draw = change frequently
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    // Start at index 0
    glEnableVertexAttribArray(0);

    // Start at 0, 4 coords per attribute, float, don't normalize, byte offset, first component offset
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

//    shaderProgram = new ShaderProgram("../assets/shaders/text.vs", "../assets/shaders/text.fs");
    shaderProgram = new ShaderProgram(TEXT_VERTEX_SHADER_PATH, TEXT_FRAGMENT_SHADER_PATH);


    // Set up projection matrix as default size.
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    shaderProgram->use();
    shaderProgram->setMat4("projection", projection);
}

/**
* Render text.
* 
* @param text                   [in] The text to display.
* 
* @param x                      [in] X coordinate.
* 
* @param y                      [in] Y coordinate.
* 
* @param scale                  [in] Font size scale.
* 
* @param color                  [in] Color.
*/

void TextRenderer::renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    shaderProgram->use();
    shaderProgram->setVec3("textColor", color);

    // TODO: Remove glActiveTexture and glBindVertexArray, since have moved to texture->use()
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Set pixel alignment to 1 byte
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        if (FT_Load_Char(face, *c, FT_LOAD_RENDER)) {
            std::cerr << "Failed to load Glyph" << std::endl;
            continue;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        GLfloat xpos = x + face->glyph->bitmap_left * scale;
        GLfloat ypos = y - (face->glyph->bitmap.rows - face->glyph->bitmap_top) * scale;

        GLfloat w = face->glyph->bitmap.width * scale;
        GLfloat h = face->glyph->bitmap.rows * scale;

        GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (face->glyph->advance.x >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Reset pixel alignment to default
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

/**
* Resize a text block.
* 
* @param width                  [in] New width.
* 
* @param height                 [in] New height.
*/

void TextRenderer::resize(int width, int height) {
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    shaderProgram->use();
    shaderProgram->setMat4("projection", projection);
}