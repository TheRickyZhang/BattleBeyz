#pragma once

#include "GameState.h"

class PauseState : public GameState {
public:
    PauseState(GameEngine* _game) : GameState(_game) {};

    void init() override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvents() override;
    void onResize(int width, int height) override;

    void update(float deltaTime) override;
    void draw() override;

    GameStateType getStateType() const override { return GameStateType::PAUSE; }
};
