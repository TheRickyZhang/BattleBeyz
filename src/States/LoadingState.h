#pragma once

#include "GameState.h"

class LoadingState : public GameState {
public:
    void init() override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvents(GameEngine* game) override;
    void update(GameEngine* game, float deltaTime) override;
    void draw(GameEngine* game) override;

    GameStateType getStateType() const override { return GameStateType::LOADING; }

private:
    float progress = 0.0f;
    const float duration = 3.0f;
};
