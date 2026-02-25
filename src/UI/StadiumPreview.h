#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "BeybladeConstants.h"
#include "FramebufferRenderer.h"
#include "Camera.h"
#include "Stadium.h"
#include "Timer.h"

class PhysicsWorld;
class ObjectShader;

class StadiumPreview {
public:
    // Provide a pointer to PhysicsWorld and ObjectShader so we can do everything internally
    StadiumPreview(int width, int height, PhysicsWorld* physicsWorld, ObjectShader* shader);

    void handleInput(float deltaTime);
    void update(float deltaTime, float currentTime);
    void draw(); // Renders the stadium into an FBO, draws ImGui image

    bool isHovered() const { return hovered; }
    Stadium* getStadium() const { return stadium.get(); }

    // Shared, because this is sometimes the preview's own stadium and sometimes one
    // owned by the active Profile. A raw pointer here double-freed on the latter.
    std::shared_ptr<Stadium> stadium;
private:
    int width;
    int height;
    bool hovered = false;

    PhysicsWorld* physicsWorld = nullptr;
    ObjectShader* objectShader = nullptr;

    // Internal resources
    std::unique_ptr<FramebufferRenderer> fbo;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Timer> stadiumRenderTimer;
};
