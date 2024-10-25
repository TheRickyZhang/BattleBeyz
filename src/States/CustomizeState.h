#pragma once

#include "GameState.h"

class CustomizeState : public GameState {
public:
    CustomizeState(GameEngine* _game) : GameState(_game) {};

    void init() override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvents() override;
    void update(float deltaTime) override;
    void draw() override;

    GameStateType getStateType() const override { return GameStateType::CUSTOMIZE; }

private:
    bool showNewProfilePopup = false;
    bool showNewBeybladePopup = false;
    char newProfileName[32] = "";
    char newBeybladeName[32] = "";

    // Temp for sliders
    float tempLayerMass = 0.020f;
    float tempLayerMOI = 0.7f * 0.027f * 0.018f * 0.018f;
    float tempDriverCOF = 0.22f;

    int currentIndex = 0;
    char currentProfileName[32] = "";
};
