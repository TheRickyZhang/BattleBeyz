#pragma once

#include <GL/glew.h>
#include <stdexcept>

class FramebufferRenderer {
private:
    GLuint fbo;
    GLuint texture;
    GLuint rbo;
    int width, height;

    void setupFramebuffer();

public:
    FramebufferRenderer(int width, int height);
    ~FramebufferRenderer();

    void bind() const;
    void unbind();
    GLuint getTexture() const;
    void resize(int newWidth, int newHeight);
};
