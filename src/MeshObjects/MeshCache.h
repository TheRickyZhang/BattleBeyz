////////////////////////////////////////////////////////////////////////////////
// MeshCache.h -- Parsed model cache -- rz -- 2026-08-25
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "MeshData.h"

/*
* Singleton. Keeps parsed geometry keyed by file path so the same .obj is read once
* however many beyblades share it.
*
* Holds no OpenGL, so prewarm() is safe on a worker thread: the loading screen parses
* every model the save file references in parallel, and the constructors that follow
* hit the cache instead of the disk.
*/
class MeshCache {
public:
    static MeshCache& getInstance();

    MeshCache(const MeshCache&) = delete;
    MeshCache& operator=(const MeshCache&) = delete;

    // Parses on a miss. Safe from any thread.
    std::shared_ptr<const MeshData> getOrParse(const std::string& path);

    // Parses into the cache and reports success. Intended as a loading task.
    bool prewarm(const std::string& path);

    bool contains(const std::string& path) const;
    size_t size() const;
    void clear();

private:
    MeshCache() = default;

    mutable std::mutex mutex;
    std::unordered_map<std::string, std::shared_ptr<const MeshData>> meshes;
};
