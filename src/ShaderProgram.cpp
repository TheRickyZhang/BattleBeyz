////////////////////////////////////////////////////////////////////////////////
// ShaderProgram.cpp -- Beyblade shader program-- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "ShaderProgram.h"
#include <iostream>
#include <fstream>
#include <sstream>

/**
* Constructor
*/

ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath) {
    // Read shader source files
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    // Compile and link shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    // Check for linking errors
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Initialize uniform locations
    modelLoc = glGetUniformLocation(ID, "model");
    viewLoc = glGetUniformLocation(ID, "view");
    projectionLoc = glGetUniformLocation(ID, "projection");
}

/**
* Destructor.
*/

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(ID);
}

/**
* Compile a shader program.
*/

GLuint ShaderProgram::compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

/**
* Get the location (offset) of a uniform.
*
* @param name                       [in] The uniform name.
*/

GLint ShaderProgram::getUniformLocation(const std::string& name) const {
    return glGetUniformLocation(ID, name.c_str());
}

/**
* Test if a named uniform item is available.
*
* @param name                   [in] Uniform name.
*
* @return The uniform location value, -1 if does not exist.
*/

bool ShaderProgram::isUniformAvailable(const std::string& name) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    return location != -1;
}

/**
* Load a shader program from a file.
*/

std::string ShaderProgram::readFile(const char* filePath)
{
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
* Set a boolean uniform.
*/

void ShaderProgram::setBool(const std::string& name, bool value) const
{
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

/**
* Set an integer uniform.
*/

void ShaderProgram::setInt(const std::string& name, int value) const
{
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

/**
* Set a uniform float value.
*
* @param name                   [in] The uniform name.
*
* @param value                  [in] A float vector.
*/

void ShaderProgram::setUniform1f(const std::string& name, float value) const
{
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

/**
* Set the material uniform.
*/

void ShaderProgram::setUniformMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

/**
* Set a uniform vec3 by name.
*
* @param name                   [in] The uniform name.
*
* @param value                  [in] A vec3 vector.
*/

void ShaderProgram::setUniformVec3(const std::string& name, const glm::vec3& value) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

/**
* Set uniforms for model, view, and projection matrices.
*/

void ShaderProgram::setUniforms(glm::mat4 model, glm::mat4 view, glm::mat4 projection) const
{
    use(); // Activate the shader program
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

/**
* Set object color.
* 
* This sets the useObjectColor flag and objectColor vectors in the shader program object.vs.
* This will override the colors defined in the vertex data.
* 
* IMPORTANT:  Turns this off when you are finished using it!
* 
* @param color                          [in] Pointer to color.  If null, the objectColor
*                                       handing is disabled.
*/

void ShaderProgram::setObjectColor(const glm::vec3* color) const
{
    setBool("useObjectColor", color == nullptr ? false : true);
    if (color != nullptr) {
        setUniformVec3("objectColor", *color);
    }
}

/**
* Update camera position and view matrix.
*/

void ShaderProgram::updateCameraPosition(const glm::vec3& cameraPosition, const glm::mat4& viewMatrix) const
{
    setUniformVec3("viewPos", cameraPosition);
    setUniformMat4("view", viewMatrix);
}

/**
* Use the current program.
*/

void ShaderProgram::use() const
{
    glUseProgram(ID);
}
