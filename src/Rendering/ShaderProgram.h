////////////////////////////////////////////////////////////////////////////////
// ShaderProgram.h -- ShaderProgram include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>


/*
* Shader source read from disk. No OpenGL, so readShaderSource() is safe from any thread;
* only the ShaderProgram constructor needs the GL context.
*/
struct ShaderSource {
    std::string vertex;
    std::string fragment;
    std::string vertexPath;    // Kept for error messages
    std::string fragmentPath;

    bool valid() const { return !vertex.empty() && !fragment.empty(); }
};

ShaderSource readShaderSource(const char* vertexPath, const char* fragmentPath);

class ShaderProgram {
public:
    GLuint ID;

    explicit ShaderProgram(const ShaderSource& source);
    virtual ~ShaderProgram();

    // Owns a GL program that the destructor deletes
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void use() const;

    // Generic setters, uses uniformCache
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec2(const std::string& name, const glm::vec2& vec) const;
    void setVec3(const std::string& name, const glm::vec3& vec) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;

    void setTint(const glm::vec3& color) const;
    //void debugUniforms(const std::vector<std::string>& uniformNames) const;

protected:
    mutable std::unordered_map<std::string, GLint> uniformCache;

    GLint getCachedUniformLocation(const std::string& name) const;
    GLuint compileShader(GLenum type, const char* source);
};
