////////////////////////////////////////////////////////////////////////////////
// Texture.h -- Texture handling include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <GL/glew.h>
#include <iostream>
#include <utility>


class Texture {
public:
    unsigned int ID{};
    std::string type;
    std::string path;
    unsigned int width, height;

    // Constructor for loading and creating a texture
    Texture(const char* imagePath, std::string texType);
    ~Texture() {
        cleanup();
    }
    void cleanup();

    // Method to bind the texture before drawing
    void use() const;
};