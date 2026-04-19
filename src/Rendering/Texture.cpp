#include <utility>
////////////////////////////////////////////////////////////////////////////////
// Texture.cpp -- Texture handling object -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "Texture.h"

/**
* Constructor. Uploads pre-decoded pixels; see decodeImage() for the CPU half.
*/

Texture::Texture(const ImageData& image, std::string sourcePath) : path(std::move(sourcePath)) {
    if (!image.valid()) {
        std::cerr << "Cannot upload invalid image data for: " << path << std::endl;
        return;
    }

    glGenTextures(1, &ID);
    if (ID == 0) {
        std::cerr << "Failed to generate texture ID for: " << path << std::endl;
        return;
    }
    width = static_cast<unsigned int>(image.width);
    height = static_cast<unsigned int>(image.height);

    glBindTexture(GL_TEXTURE_2D, ID);

    // Set texture wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGB;
    switch (image.channels) {
        case 1: format = GL_RED;  break;
        case 2: format = GL_RG;   break;
        case 3: format = GL_RGB;  break;
        case 4: format = GL_RGBA; break;
        default:
            std::cerr << "Unsupported channel count " << image.channels << " for: " << path << std::endl;
            break;
    }

    // stb hands back tightly packed rows, which the default alignment of 4 would skew
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, image.width, image.height, 0, format,
                 GL_UNSIGNED_BYTE, image.pixels.get());
    glGenerateMipmap(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    std::cout << "Loaded texture: " << path << " with dimensions: " << image.width << "x"
              << image.height << " and channels: " << image.channels << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done to prevent accidental modification
}

/**
* Enable texture use.
*/

void Texture::use(int textureUnit) const {
    // TOLOOK: Different texture unit values, but only 0 is sufficient for now
    glActiveTexture(GL_TEXTURE0 + textureUnit); // Activate the specified texture unit
    glBindTexture(GL_TEXTURE_2D, ID);           // Bind the texture to GL_TEXTURE_2D
}

/**
* Texture cleanup.
*/

void Texture::cleanup() {
    if (ID != 0) {
        glDeleteTextures(1, &ID);
        ID = 0;
    }
}
