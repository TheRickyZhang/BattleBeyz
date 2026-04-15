////////////////////////////////////////////////////////////////////////////////
// test_mesh_parsing.cpp -- OBJ parsing, which runs off the GL thread
////////////////////////////////////////////////////////////////////////////////

#include <doctest/doctest.h>

#include <filesystem>

#include "MeshCache.h"
#include "MeshData.h"

// parseObj() touches no OpenGL, so the real assets can be parsed here. This is also
// the guard on the pinned tinyobjloader commit: a bad bump shows up as a parse
// failure rather than as a broken model at runtime.

namespace {
    // Tests run from the build directory, which CMake copies assets into, but running
    // the binary from the repo root should work too.
    std::string modelPath(const std::string& name) {
        std::filesystem::path local = std::filesystem::path("assets/models") / name;
        return local.string();
    }
}

TEST_CASE("the default model parses into usable geometry") {
    MeshData mesh = parseObj(modelPath("default.obj"));

    REQUIRE(mesh.loaded);
    CHECK_FALSE(mesh.vertices.empty());
    CHECK_FALSE(mesh.indices.empty());

    // Every index has to address a real vertex or the upload draws garbage
    for (unsigned int index : mesh.indices) {
        REQUIRE(index < mesh.vertices.size());
    }

    // Triangulated, so indices come in threes
    CHECK(mesh.indices.size() % 3 == 0);
}

TEST_CASE("parsed attributes line up with the vertex list") {
    MeshData mesh = parseObj(modelPath("default.obj"));
    REQUIRE(mesh.loaded);

    CHECK(mesh.normals.size() == mesh.vertices.size());
    CHECK(mesh.texCoords.size() == mesh.vertices.size());
}

TEST_CASE("the bounding box encloses every vertex") {
    MeshData mesh = parseObj(modelPath("default.obj"));
    REQUIRE(mesh.loaded);

    for (const glm::vec3& v : mesh.vertices) {
        REQUIRE(v.x >= mesh.boundingBox.min.x);
        REQUIRE(v.y >= mesh.boundingBox.min.y);
        REQUIRE(v.z >= mesh.boundingBox.min.z);
        REQUIRE(v.x <= mesh.boundingBox.max.x);
        REQUIRE(v.y <= mesh.boundingBox.max.y);
        REQUIRE(v.z <= mesh.boundingBox.max.z);
    }
}

TEST_CASE("a missing file fails rather than throwing") {
    MeshData mesh = parseObj(modelPath("this-model-does-not-exist.obj"));
    CHECK_FALSE(mesh.loaded);
}

TEST_CASE("the cache returns one parse for repeated paths") {
    MeshCache& cache = MeshCache::getInstance();
    cache.clear();

    std::shared_ptr<const MeshData> first = cache.getOrParse(modelPath("default.obj"));
    REQUIRE(first != nullptr);
    REQUIRE(cache.size() == 1);

    std::shared_ptr<const MeshData> second = cache.getOrParse(modelPath("default.obj"));
    CHECK(second == first);       // same object, not a re-parse
    CHECK(cache.size() == 1);

    cache.clear();
    CHECK(cache.size() == 0);
}
