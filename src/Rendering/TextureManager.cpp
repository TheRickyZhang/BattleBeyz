#include "TextureManager.h"
#include "Texture.h"

// Singleton access - ensures only one instance of TextureManager
TextureManager& TextureManager::getInstance() {
    static TextureManager instance;
    return instance;
}

// Loads a texture and returns a shared pointer to it
std::shared_ptr<Texture> TextureManager::loadTexture(const std::string& name, const std::string& filePath) {
    std::lock_guard<std::mutex> lock(mutex);

    // If the texture already loaded, return ppointer, else call constructor
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }
    std::shared_ptr<Texture> texture = std::make_shared<Texture>(filePath.c_str(), name);

    textures[name] = texture;
    return texture;
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
    textures.erase(name);
}

// Clears all loaded textures
void TextureManager::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    textures.clear();
}
