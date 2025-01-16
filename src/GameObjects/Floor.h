#pragma once

#include "MeshObject.h"

class Floor : public MeshObject {
public:
    Floor(float w, float d, float y, float centerX = 0.0f, float centerZ = 0.0f);

    void updateMesh() override;

private:
    glm::vec3 center;
};
