////////////////////////////////////////////////////////////////////////////////
// ImageData.cpp -- CPU-side image decoding -- rz -- 2026-08-25
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

// Makes stbi_failure_reason() per-thread, so concurrent decodes report correctly
#define STBI_THREAD_LOCAL thread_local
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ImageData.h"

void StbImageDeleter::operator()(unsigned char* pixels) const noexcept {
    stbi_image_free(pixels);
}

ImageData decodeImage(const std::string& path) {
    ImageData image;
    image.pixels.reset(stbi_load(path.c_str(), &image.width, &image.height, &image.channels, 0));

    if (!image.pixels) {
        std::cerr << "Failed to decode image: " << path << " (" << stbi_failure_reason() << ")" << std::endl;
        image.width = image.height = image.channels = 0;
    }
    return image;
}
