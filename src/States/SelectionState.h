#pragma once

#include <vector>
#include <memory>
#include <string>

#include "GameState.h"

class Profile;
class Beyblade;

class SelectionState : public GameState {
public:
    SelectionState(GameEngine* _game) : GameState(_game) {};

    void init() override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvents() override;
    void update(float deltaTime) override;
    void draw() override;

    GameStateType getStateType() const override { return GameStateType::SELECTION; }

private:
    
};
