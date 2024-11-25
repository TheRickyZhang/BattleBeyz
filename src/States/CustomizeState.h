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
    enum class PopupState {
        NONE,
        NEW_PROFILE,
        NEW_BEYBLADE,
        DELETE_PROFILE,
        DELETE_BEYBLADE
    };
    PopupState currentPopup = PopupState::NONE;

    char newProfileName[32] = "";
    char newBeybladeName[32] = "";
    std::string currentProfileName = "";
    std::string currentBeybladeName = "";

    // 2024-11-18.  When a new blade is selected we must refresh the tempxxx values:

    BeybladeBody* prevbladeBody = nullptr;

    // Temp for sliders
    float tempLayerMass = 0.020f;
    float tempLayerMOI = 0.7f * 0.027f * 0.018f * 0.018f;
    float tempDriverCOF = 0.22f;

};
