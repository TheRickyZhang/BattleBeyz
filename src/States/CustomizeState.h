#pragma once

#include "GameState.h"
#include "GameEngine.h"
#include "Profile.h"
#include "Beyblade.h"
#include "BeybladeBody.h"
#include <vector>
#include <memory>
#include <string>

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

    // Helper Methods
    void precomputeLayout(float& windowWidth, float& frameSpacing, float& spacing,
        float& leftTextWidth, float& rightButton1X, float& rightButton2X,
        float& dropdownLeftX, float& dropdownWidth);

    void initializeData(std::vector<std::shared_ptr<Profile>>& profiles,
        std::vector<std::shared_ptr<Beyblade>>& beyblades,
        std::shared_ptr<Beyblade>& beyblade);

    void drawProfileSection(const std::vector<std::shared_ptr<Profile>>& profiles,
        float dropdownLeftX, float dropdownWidth,
        float rightButton1X, float rightButton2X);

    void drawBeybladeSection(const std::vector<std::shared_ptr<Beyblade>>& beyblades,
        float dropdownLeftX, float dropdownWidth,
        float rightButton1X, float rightButton2X);

    void drawManualCustomizeSection(std::shared_ptr<Beyblade> beyblade);
    void drawTemplateCustomizeSection(std::shared_ptr<Beyblade> beyblade);

    void drawPopups();

    // Member Variables
    PopupState currentPopup = PopupState::NONE;
    bool isTemplate = false;

    char newProfileName[32] = "";
    char newBeybladeName[32] = "";
    std::string currentProfileName = "";
    std::string currentBeybladeName = "";

    BeybladeBody* prevbladeBody = nullptr;
    
    // Temporary variables to edit with
    // Templated
    int tempSelectedLayer = -1;
    int tempSelectedDisc = -1;
    int tempSelectedDriver = -1;

    // Manual
    float tempLayerMass = 0.020f;
    float tempLayerMOI = 0.7f * 0.027f * 0.018f * 0.018f;
    float tempDriverCOF = 0.22f;
};
