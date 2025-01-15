////////////////////////////////////////////////////////////////////////////////
// Texture.h -- Texture handling include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#include <string>

#include <GL/glew.h>

// TODO: Create Material class if needed
class Texture {
public:
    unsigned int ID{};
    std::string path;
    unsigned int width{}, height{};

    // Constructor for loading and creating a texture
    Texture(const char* imagePath);
    ~Texture() {
        cleanup();
    }
    void cleanup();

    // Method to bind the texture before drawing
    void use(int textureUnit = 0) const;
};