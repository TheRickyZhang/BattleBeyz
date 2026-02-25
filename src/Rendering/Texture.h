////////////////////////////////////////////////////////////////////////////////
// Texture.h -- Texture handling include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#include <string>

#include <GL/glew.h>

#include "ImageData.h"

// TODO: Create Material class if needed
class Texture {
public:
    unsigned int ID{};
    std::string path;
    unsigned int width{}, height{};

    // Uploads already-decoded pixels. Must run on the thread holding the GL context;
    // use decodeImage() to produce the ImageData anywhere.
    Texture(const ImageData& image, std::string sourcePath = {});
    ~Texture() {
        cleanup();
    }

    // Owns a GL handle that the destructor deletes, so copying would double-delete
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    void cleanup();

    // Method to bind the texture before drawing
    void use(int textureUnit = 0) const;
};
