////////////////////////////////////////////////////////////////////////////////
// ImageData.h -- CPU-side decoded image -- rz -- 2026-08-25
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <string>

struct StbImageDeleter {
    void operator()(unsigned char* pixels) const noexcept;
};

/*
* Decoded pixels with no OpenGL attached. Texture consumes one of these to upload.
* Keeping the two apart is what lets decoding happen off the GL thread.
*/
struct ImageData {
    int width = 0;
    int height = 0;
    int channels = 0;
    std::unique_ptr<unsigned char, StbImageDeleter> pixels;

    bool valid() const { return pixels != nullptr && width > 0 && height > 0; }
};

// File read plus decode. No OpenGL, safe to call from any thread.
ImageData decodeImage(const std::string& path);
