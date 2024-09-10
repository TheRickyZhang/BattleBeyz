////////////////////////////////////////////////////////////////////////////////
// ShaderProgram.h -- ShaderProgram include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class ShaderProgram {
public:
    GLuint ID;
    GLint modelLoc, viewLoc, projectionLoc;

    ShaderProgram(const char* vertexPath, const char* fragmentPath);
    ~ShaderProgram();

    void setBool(const std::string& name, bool value) const;
    void setObjectColor(const glm::vec3* color) const;
    void setUniformMat4(const std::string& name, const glm::mat4& mat) const;
    void setUniformVec3(const std::string& name, const glm::vec3& value) const;
    void setUniform1f(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
    void setUniforms(glm::mat4 model, glm::mat4 view, glm::mat4 projection) const;
    void updateCameraPosition(const glm::vec3& cameraPosition, const glm::mat4& viewMatrix) const;
    void use() const;

private:
    GLuint compileShader(GLenum type, const char* source);
    [[nodiscard]] GLint getUniformLocation(const std::string &name) const;
    bool isUniformAvailable(const std::string& name) const;
    static std::string readFile(const char* filePath);
};