#pragma once

#include "GameState.h"
#include "Stadium.h"
#include "Beyblade.h"
#include "QuadRenderer.h"
#include "Floor.h"

class PreBattleState : public GameState {
public:
    PreBattleState(GameEngine* game,
        std::vector<std::shared_ptr<Stadium>> stadiums,
        std::vector<std::shared_ptr<Beyblade>> beyblades)
        : GameState(game),
        stadiums(std::move(stadiums)),
        beyblades(std::move(beyblades)) {
    }

    void init() override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvents() override;
    void onResize(int width, int height) override;

    void update(float deltaTime) override;
    void draw() override;

    GameStateType getStateType() const override { return GameStateType::PREBATTLE; }

private:
    bool showInfoScreen = true;

    float imguiColor[3] = { 0.45f, 0.55f, 0.60f };

    Floor* floor{};

    // NOTE that these raw pointers are only valid as long as the original unique ptrs are not deleted
    std::vector<std::shared_ptr<Stadium>> stadiums;  // Shared ownership of stadiums
    std::vector<std::shared_ptr<Beyblade>> beyblades; // Shared ownership of beyblades

    // Launch parameters, edited here and applied to the bodies on Launch.
    // These are the source of truth while positioning; the bodies follow them.
    struct LaunchSettings {
        glm::vec3 center{};
        glm::vec3 velocity{};
        glm::vec3 angularVelocity{};
    };
    std::vector<LaunchSettings> launchSettings;  // Parallel to beyblades

    // For interactivity
    int heldBeyblade = -1;  // Index into beyblades, -1 when nothing is held
    glm::vec3 dragOffset;
    const float stadiumY = 0.0f;

    void drawInfoScreen();
};
