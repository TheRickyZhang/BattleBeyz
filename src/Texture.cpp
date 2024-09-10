////////////////////////////////////////////////////////////////////////////////
// Texture.cpp -- Texture handling object -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/**
* Constructor.
*/

Texture::Texture(const char* imagePath, std::string texType) : type(std::move(texType)), path(imagePath) {
    std::cout << "Loading texture: " << imagePath << std::endl; // Add this line

    glGenTextures(1, &ID);
    if (ID == 0) {
        std::cerr << "Failed to generate texture ID for: " << imagePath << std::endl;
        return;
    }
    glBindTexture(GL_TEXTURE_2D, ID);

    // Set texture wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image data
    int w, h, nrChannels;

//    stbi_set_flip_vertically_on_load(true);  // Flip image vertically

    unsigned char* data = stbi_load(imagePath, &w, &h, &nrChannels, 0);
    width = w;
    height = h;
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);  // Generate mipmaps for the texture
        std::cout << "Loaded texture: " << imagePath << " with dimensions: " << w << "x" << h << " and channels: " << nrChannels << std::endl;
    } else {
        std::cout << "Failed to load texture: " << imagePath << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done to prevent accidental modification
}

/**
* Enable texture use.
*/

void Texture::use() const {
    glBindTexture(GL_TEXTURE_2D, ID);
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
