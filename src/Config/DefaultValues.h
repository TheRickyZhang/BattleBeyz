#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "ShaderPath.h"
#include "Texture.h"

namespace Colors {
    inline const glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
    inline const glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);
    inline const glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
    inline const glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
    inline const glm::vec3 black = glm::vec3(0.0f, 0.0f, 0.0f);
    inline const glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
    inline const glm::vec3 cyan = glm::vec3(0.0f, 1.0f, 1.0f);
    inline const glm::vec3 magenta = glm::vec3(1.0f, 0.0f, 1.0f);
    inline const glm::vec3 gray = glm::vec3(0.5f, 0.5f, 0.5f);
}

// Shifted = Multiplicative range, non-shifted = fixed range
namespace StadiumDefaults {
    using namespace glm;

    constexpr char* name = "Default Stadium";

    inline vec3 center             = vec3(0.0f);   inline vec3 centerMin = vec3(-10.0f);         inline vec3 centerMax = vec3(10.0f);
      constexpr float radius      = 1.2f;           constexpr float radiusMin = radius / 4;      constexpr float radiusMax = radius * 4;
      constexpr float curvature   = 0.10f;          constexpr float curvatureMin = curvature/4;  constexpr float curvatureMax = curvature*4;
    constexpr float COF           = 0.35f;        constexpr float COFMin = 0;                  constexpr float COFMax = 1;
    constexpr int verticesPerRing = 64;           constexpr int verticesPerRingMin = 16;       constexpr int verticesPerRingMax = 1024;
    constexpr int numRings        = 8;            constexpr int numRingsMin = 4;               constexpr int numRingsMax = 32;
    inline vec3 ringColor  = Colors::red;          inline vec3 ringColorMin = Colors::black;     inline vec3 ringColorMax = Colors::black; const
    inline vec3 crossColor = Colors::blue;         inline vec3 crossColorMin = Colors::black;    inline vec3 crossColorMax = Colors::white;
    inline vec3 tint       = Colors::white;        inline vec3 tintMin = Colors::black;          inline vec3 tintMax = Colors::white;
    constexpr float textureScale  = 1.0f;         constexpr float textureScaleMin = 0.1f;      constexpr float textureScaleMax = 10.0f;
}

inline std::shared_ptr<Texture> DefaultTexture() {
    static std::shared_ptr<Texture> texture = std::make_shared<Texture>(MISSING_TEXTURE_PATH);
    return texture;
}
