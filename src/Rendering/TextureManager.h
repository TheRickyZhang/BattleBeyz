#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

// Since Textures should ALWAYS be interacted with through TextureManager
#include "Texture.h"

// Manages string->Texture* map
class TextureManager {
public:
    static TextureManager& getInstance();
    
    // Deleted for singleton
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    std::shared_ptr<Texture> loadTexture(const std::string& name, const std::string& texturePath);
    std::shared_ptr<Texture> getTexture(const std::string& texturePath) const;

    void unloadTexture(const std::string& name);
    void clear();
private:
    TextureManager() = default;
    mutable std::mutex mutex;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
};