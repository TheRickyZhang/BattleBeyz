#pragma once

#include "StateIdentifiers.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class GameEngine;

class GameState {
public:
    virtual ~GameState() = default;

    virtual void init() = 0;
    virtual void cleanup() = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;

    virtual void handleEvents(GameEngine* game) = 0;
    virtual void update(GameEngine* game, float deltaTime) = 0;
    virtual void draw(GameEngine* game) = 0;

    virtual GameStateType getStateType() const = 0;
};
