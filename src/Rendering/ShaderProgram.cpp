////////////////////////////////////////////////////////////////////////////////
// ShaderProgram.cpp -- Beyblade shader program-- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "ShaderProgram.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>

/**
 * Constructor for loading and linking vertex and fragment shaders.
 * 
 * @param vertexPath        Vertex shader source code
 * @param fragmentPath      Fragment shader source code
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

    use();
    //debugActiveAttributes(ID);
}

// Destructor
ShaderProgram::~ShaderProgram() {
    glDeleteProgram(ID);
}


/**
 * This function updates the transformation and camera-related uniforms in the shader.
 *
 * @param model            Model transformation matrix.
 * @param view             View matrix (camera transformation).
 * @param projection       Projection matrix (perspective or orthographic).
 * @param cameraPosition   World-space position of the camera.
 */
void ShaderProgram::setRenderMatrices(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPosition) const {
    setMat4("model", model);
    setMat4("view", view);
    setMat4("projection", projection);
    setVec3("viewPos", cameraPosition);
}


/**
 * Set the light properties for the shader, including type, color, and position. See object.vs for the type ENUM.
 *
 * @param lightType     0 for directional, 1 for point light
 * @param lightColor    RGB color of the light source.
 * @param lightPos      Position or direction of the light in world space.
 */
void ShaderProgram::setLight(LightType lightType, const glm::vec3& lightColor, const glm::vec3& lightPos) const {
    setInt("lightType", static_cast<int>(lightType));
    setVec3("lightColor", lightColor);
    setVec3("lightPos", lightPos);
}

// Update based on camera movement
void ShaderProgram::setCameraView(const glm::vec3& cameraPosition, const glm::mat4& viewMatrix) const
{
    setVec3("viewPos", cameraPosition);
    setMat4("view", viewMatrix);
}


/**
 * Applies tint to all vertices.
 Should only be used when a distinct tint is necessary, NOT for applying vertex colors
 *
 * @param tint      RGB tint color (default is vec3(1.0, 1.0, 1.0) for no tint).
 */
void ShaderProgram::setTint(const glm::vec3& tint) const {
    setVec3("tint", tint);
}

/**
 * Compile a shader of the specified type using the provided source code.
 *
 * @param type      Type of the shader (e.g., GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
 * @param source    Shader source code
 *
 * @return The compiled shader object ID. Returns 0 if compilation fails.
 */
GLuint ShaderProgram::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED ["
            << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "]\n"
            << infoLog << std::endl;
    }
    return shader;
}


/**
 * Read the entire contents of a file and return it as a string.
 * Typically used to load shader source code from disk.
 *
 * @param filePath   Path to the file.
 *
 * @return A string containing the file contents.
 */
std::string ShaderProgram::readFile(const char* filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Activate the shader program for rendering.
void ShaderProgram::use() const {
    if (ID != 0) glUseProgram(ID);
}

//bool ShaderProgram::isUniformAvailable(const std::string& name) const
//{
//    GLint location = glGetUniformLocation(ID, name.c_str());
//    return location != -1;
//}

/**
* Set uniforms for model, view, and projection matrices.
*/

//void ShaderProgram::setUniforms(glm::mat4 model, glm::mat4 view, glm::mat4 projection) const
//{
//    use(); // Activate the shader program
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
//    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
//}


/**
 * Set a float uniform.
 *
 * @param name   Name of the uniform variable in the shader.
 * @param value  Float value to set.
 */


/* Private Methods */
GLint ShaderProgram::getCachedUniformLocation(const std::string& name) const {
    // Check cache first
    if (uniformCache.find(name) != uniformCache.end()) {
        return uniformCache[name];
    }

    // Fetch from OpenGL and cache it
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        uniformCache[name] = location;
    }
    else {
        std::cerr << "WARNING::UNIFORM::NOT_FOUND::" << name << std::endl;
    }
    return location;
}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }
    else {
        std::cerr << "Mat4 " << name << std::endl;
        //debugActiveAttributes(ID);
    }
}

void ShaderProgram::setVec3(const std::string& name, const glm::vec3& vec) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(vec));
    }
    else {
        std::cerr << "Vec3 " << name << std::endl;
        //debugActiveAttributes(ID);
    }
}

void ShaderProgram::setFloat(const std::string& name, float value) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1f(location, value);
    }
    else {
        std::cerr << "Float " << name << std::endl;
        //debugActiveAttributes(ID);
    }
}

void ShaderProgram::setInt(const std::string& name, int value) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1i(location, value);
    }
    else {
        std::cerr << "Int " << name << std::endl;
        //debugActiveAttributes(ID);
    }
}
//void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const {
//    GLint location = getCachedUniformLocation(name);
//    if (location != -1) {
//        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
//    }
//}
//
//void ShaderProgram::setVec3(const std::string& name, const glm::vec3& vec) const {
//    GLint location = getCachedUniformLocation(name);
//    if (location != -1) {
//        glUniform3fv(location, 1, glm::value_ptr(vec));
//    }
//}
//
//void ShaderProgram::setFloat(const std::string& name, float value) const {
//    GLint location = getCachedUniformLocation(name);
//    if (location != -1) {
//        glUniform1f(location, value);
//    }
//}
//
//void ShaderProgram::setInt(const std::string& name, int value) const {
//    GLint location = getCachedUniformLocation(name);
//    if (location != -1) {
//        glUniform1i(location, value);
//    }
//}




void ShaderProgram::debugUniforms(const std::vector<std::string>& uniformNames) const {
    std::cout << "Debugging Uniform Locations for Shader Program (ID: " << static_cast<int>(ID) << "):" << std::endl;

    for (const auto& name : uniformNames) {
        GLint location = glGetUniformLocation(ID, name.c_str());
        if (location == -1) {
            std::cerr << "WARNING::UNIFORM::NOT_FOUND::" << name << std::endl;
        }
        else {
            std::cout << name << ": " << location << std::endl;
        }
    }
}


void ShaderProgram::validateUniforms(const std::vector<std::string>& uniformNames) const {
    std::cout << "Validating Uniforms for Shader Program (ID: " << static_cast<int>(ID) << "):" << std::endl;

    for (const auto& name : uniformNames) {
        GLint location = glGetUniformLocation(ID, name.c_str());
        if (location == -1) {
            std::cerr << "WARNING::UNIFORM::NOT_FOUND::" << name << std::endl;
        }
        else {
            std::cout << "Uniform '" << name << "' found at location: " << location << std::endl;
        }
    }
}