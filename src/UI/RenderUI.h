#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "FramebufferRenderer.h"
#include "Camera.h"
#include "Stadium.h"
#include "Timer.h"

class ObjectShader;


class StadiumPreview {
public:
    StadiumPreview(int width, int height, PhysicsWorld* physicsWorld);

    void init();
    void update(float deltaTime, float currentTime) const;
    bool draw(ObjectShader& shader);

    int previewWidth, previewHeight;
    std::unique_ptr<FramebufferRenderer> stadiumRenderer;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Stadium> stadium;
    std::unique_ptr<Timer> stadiumRenderTimer;
    bool isMouseHovering = false;
};

