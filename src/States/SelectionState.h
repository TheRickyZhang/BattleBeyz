#pragma once

#include <vector>
#include <memory>
#include <string>

#include "GameState.h"
#include "Profile.h"

class Beyblade;
class PhysicsWorld;

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

    void drawPlayerPanels(const std::vector<std::shared_ptr<Profile>>& players, int leftPanelWidth, int rightPanelWidth);

    GameStateType getStateType() const override { return GameStateType::SELECTION; }

private:
    void precomputeLayout(int& leftPanelWidth, int& rightPanelWidth);
    std::vector<Profile> profiles;
    
    float tempGravity = 0.0f;
};
