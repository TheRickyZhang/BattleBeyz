#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "FramebufferRenderer.h"
#include "Camera.h"
#include "Stadium.h"
#include "Timer.h"

class PhysicsWorld;
class ObjectShader;

// TODO: Remove width and height paramters, pass into draw()
class StadiumPreview {
public:
    // Provide a pointer to PhysicsWorld and ObjectShader so we can do everything internally
    StadiumPreview(int width, int height, PhysicsWorld* physicsWorld, ObjectShader* shader, std::shared_ptr<Stadium> stadium = nullptr);

    void handleInput(float deltaTime);
    void update(float deltaTime, float currentTime);
    void draw(); // Renders the stadium into an FBO, draws ImGui image

    bool isHovered() const { return hovered; }
    std::shared_ptr<Stadium> getStadium() { return stadium; }
    void setStadium(std::shared_ptr<Stadium> s) { stadium = s; }

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
    std::shared_ptr<Stadium> stadium;
};
