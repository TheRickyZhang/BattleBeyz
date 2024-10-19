#pragma once

#include "GameState.h"

class CustomizeState : public GameState {
public:
    void init() override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvents(GameEngine* game) override;
    void update(GameEngine* game, float deltaTime) override;
    void draw(GameEngine* game) override;

    GameStateType getStateType() const override { return GameStateType::CUSTOMIZE; }

private:
    bool showNewProfilePopup = false;
    bool showNewBeybladePopup = false;
    char newProfileName[32] = "";
    char newBeybladeName[32] = "";

    // Temp for sliders
    float tempLayerMass = 0.020f;
    float tempLayerMOI = 0.7 * 0.027 * 0.018 * 0.018f;
    float tempDriverCOF = 0.22f;

    int currentIndex = 0;
    char currentProfileName[32] = "";
};
