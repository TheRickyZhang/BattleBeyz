#pragma once

#include "GameState.h"
#include "Stadium.h"
#include "Beyblade.h"
#include "QuadRenderer.h"
#include "Floor.h"

class ActiveState : public GameState {
public:
    ActiveState(GameEngine* game,
        std::vector<std::shared_ptr<Stadium>> stadiums,
        std::vector<std::shared_ptr<Beyblade>> beyblades,
        std::shared_ptr<PhysicsWorld> physicsWorld)
        : GameState(game),
        stadiums(std::move(stadiums)),
        beyblades(std::move(beyblades)),
        physicsWorld(std::move(physicsWorld)) {
    }

    void init() override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvents() override;
    void update(float deltaTime) override;
    void draw() override;

    GameStateType getStateType() const override { return GameStateType::ACTIVE; }

private:
    bool showInfoScreen = true;

    float imguiColor[3] = { 0.45f, 0.55f, 0.60f };

    Floor* floor{};

    // NOTE that these raw pointers are only valid as long as the original unique ptrs are not deleted
    std::vector<std::shared_ptr<Stadium>> stadiums;  // Shared ownership of stadiums
    std::vector<std::shared_ptr<Beyblade>> beyblades; // Shared ownership of beyblades
    std::shared_ptr<PhysicsWorld> physicsWorld;       // Shared ownership of physics world

    void drawInfoScreen();
};
