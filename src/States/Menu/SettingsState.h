#pragma once

#include "GameState.h"

class SettingsState : public GameState {
public:
    SettingsState(GameEngine* _game) : GameState(_game) {};

    void init() override;
    void cleanup() override;
    void pause() override;
    void resume() override;
    void handleEvents() override;
    void onResize(int width, int height) override;

    void update(float deltaTime) override;
    void draw() override;

    GameStateType getStateType() const override { return GameStateType::ACTIVE; }

private:
    
};
