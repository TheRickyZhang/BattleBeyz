////////////////////////////////////////////////////////////////////////////////
// ShaderProgram.h -- ShaderProgram include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>


class ShaderProgram {
public:
    GLuint ID;

    ShaderProgram(const char* vertexPath, const char* fragmentPath);
    virtual ~ShaderProgram();

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
    static std::string readFile(const char* filePath);
};
