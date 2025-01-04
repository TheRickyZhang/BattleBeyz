////////////////////////////////////////////////////////////////////////////////
// ObjectShader.cpp -- ObjectShader implementation -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "ObjectShader.h"

ObjectShader::ObjectShader(const char* vertexPath, const char* fragmentPath)
    : ShaderProgram(vertexPath, fragmentPath) {
}

void ObjectShader::setGlobalRenderParams(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos) const {
    setMat4("view", view);
    setMat4("projection", projection);
    setVec3("viewPos", viewPos);
}

void ObjectShader::setObjectRenderParams(const glm::mat4& model, const glm::vec3& tint) const {
    setMat4("model", model);
    setVec3("tint", tint);
}

void ObjectShader::setLight(LightType lightType, const glm::vec3& lightColor, const glm::vec3& lightPos) const {
    setInt("lightType", static_cast<int>(lightType));
    setVec3("lightColor", lightColor);
    setVec3("lightPos", lightPos);
}
