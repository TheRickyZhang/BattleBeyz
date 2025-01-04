#pragma once

#include "GameObject.h"

class Floor : public GameObject {
public:
    Floor(float w, float d, float y, float centerX = 0.0f, float centerZ = 0.0f);

    void initializeMesh() override;

private:
    glm::vec3 center;
};
