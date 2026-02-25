////////////////////////////////////////////////////////////////////////////////
// BoundingBoxRenderer.h -- Debug drawing for BoundingBox -- rz -- 2026-08-25
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>

class BoundingBox;
class ObjectShader;

/*
* Draws bounding boxes in wireframe for debug mode.
*
* One set of buffers is reused for every box: the index layout is constant and only
* the eight corners change, so each draw is a glBufferSubData rather than the
* glGen/glBufferData-per-box-per-frame the old BoundingBox::renderDebug did.
*/
class BoundingBoxRenderer {
public:
    BoundingBoxRenderer();
    ~BoundingBoxRenderer();

    BoundingBoxRenderer(const BoundingBoxRenderer&) = delete;
    BoundingBoxRenderer& operator=(const BoundingBoxRenderer&) = delete;

    // Call once around a group of draw() calls; restores fill mode on end
    void begin(ObjectShader& shader);
    void draw(ObjectShader& shader, const BoundingBox& box, const glm::vec3& bodyPosition);
    void end();

private:
    void ensureBuffers();

    unsigned int VAO{}, VBO{}, EBO{};
};
