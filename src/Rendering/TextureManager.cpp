#include <utility>
#include "TextureManager.h"
#include "Texture.h"

// Singleton access - ensures only one instance of TextureManager
TextureManager& TextureManager::getInstance() {
    static TextureManager instance;
    return instance;
}

// Loads a texture and returns a shared pointer to it
std::shared_ptr<Texture> TextureManager::loadTexture(const std::string& name, const std::string& filePath) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = textures.find(name);
        if (it != textures.end()) {
            return it->second;
        }
        // Already loaded under a different name; alias rather than upload again
        auto byPath = texturesByPath.find(filePath);
        if (byPath != texturesByPath.end()) {
            return textures.emplace(name, byPath->second).first->second;
        }
    }

    // Decode outside the lock: it is the slow half and touches nothing shared.
    // The GL upload still has to happen on the context thread.
    ImageData image = decodeImage(filePath);
    std::shared_ptr<Texture> texture = std::make_shared<Texture>(image, filePath);

    std::lock_guard<std::mutex> lock(mutex);
    // Another caller may have inserted the same file while we were decoding
    auto stored = texturesByPath.emplace(filePath, std::move(texture)).first->second;
    return textures.emplace(name, std::move(stored)).first->second;
}

std::shared_ptr<Texture> TextureManager::getTexture(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }
    std::cerr << "Texture not found: " << name << std::endl;
    return nullptr;
}

// Unloads a texture from memory by removing it from the map
void TextureManager::unloadTexture(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = textures.find(name);
    if (it == textures.end()) return;
    texturesByPath.erase(it->second->path);
    textures.erase(it);
}

// Clears all loaded textures
void TextureManager::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    textures.clear();
    texturesByPath.clear();
}
