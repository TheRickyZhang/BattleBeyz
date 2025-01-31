////////////////////////////////////////////////////////////////////////////////
// BackgroundShader.cpp -- BackgroundShader implementation -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "BackgroundShader.h"

BackgroundShader::BackgroundShader(const char* vertexPath, const char* fragmentPath)
    : ShaderProgram(vertexPath, fragmentPath) {
}

void BackgroundShader::setBackgroundGlobalParams(const glm::mat4& projection, float wrapFactor, float time) const {
    use();
    setMat4("projection", projection);
    setFloat("wrapFactor", wrapFactor);
    setFloat("time", time);
}

void BackgroundShader::setBackgroundObjectParams(const glm::mat4& model, GLuint textureID) const {
    use();
    setMat4("model", model);
    setInt("backgroundTexture", textureID);
}
