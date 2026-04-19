#include <utility>
////////////////////////////////////////////////////////////////////////////////
// MeshCache.cpp -- Parsed model cache -- rz -- 2026-08-25
////////////////////////////////////////////////////////////////////////////////

#include "MeshCache.h"

MeshCache& MeshCache::getInstance() {
    static MeshCache instance;
    return instance;
}

std::shared_ptr<const MeshData> MeshCache::getOrParse(const std::string& path) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = meshes.find(path);
        if (it != meshes.end()) return it->second;
    }

    // Parse outside the lock; two threads racing on the same path both parse, and the
    // first to finish wins the insert. Wasteful only in a race we do not provoke.
    auto parsed = std::make_shared<const MeshData>(parseObj(path));

    std::lock_guard<std::mutex> lock(mutex);
    return meshes.emplace(path, std::move(parsed)).first->second;
}

bool MeshCache::prewarm(const std::string& path) {
    return getOrParse(path)->loaded;
}

bool MeshCache::contains(const std::string& path) const {
    std::lock_guard<std::mutex> lock(mutex);
    return meshes.find(path) != meshes.end();
}

size_t MeshCache::size() const {
    std::lock_guard<std::mutex> lock(mutex);
    return meshes.size();
}

void MeshCache::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    meshes.clear();
}
