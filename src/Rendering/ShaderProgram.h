////////////////////////////////////////////////////////////////////////////////
// ShaderProgram.h -- ShaderProgram include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>

enum LightType {
    Directional = 0,
    Point = 1,
    // Future: Spotlight = 2
};

class ShaderProgram {
public:
    GLuint ID;

    ShaderProgram(const char* vertexPath, const char* fragmentPath);
    ~ShaderProgram();

    // Specific setters
    void setRenderMatrices(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPosition) const;
    void setLight(LightType lightType, const glm::vec3& lightColor, const glm::vec3& lightPos) const;
    void setCameraView(const glm::vec3& cameraPosition, const glm::mat4& viewMatrix) const;
    void setTint(const glm::vec3& tint) const;

    void use() const;

    // Generic setters, uses uniformCache
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& vec) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;

    //void debugUniforms(const std::vector<std::string>& uniformNames) const;

private:
    mutable std::unordered_map<std::string, GLint> uniformCache;

    GLint getCachedUniformLocation(const std::string& name) const;
    GLuint compileShader(GLenum type, const char* source);
    static std::string readFile(const char* filePath);
};