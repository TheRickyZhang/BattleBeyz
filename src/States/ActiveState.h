#pragma once

#include "GameState.h"
#include "Stadium.h"
#include "Beyblade.h"
#include "QuadRenderer.h"

class ActiveState : public GameState {
public:
    ActiveState(GameEngine* _game) : GameState(_game) {};

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

    GLuint floorVAO{}, floorVBO{}, floorEBO{}; // TODO: floor class? Right now assum only one floor;
    QuadRenderer* floor{};
    std::vector<Stadium*> stadiums;
    std::vector<Beyblade*> beyblades;

    void drawInfoScreen();
};
