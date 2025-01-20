#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "DefaultValues.h"

#include "Camera.h"
#include "GameState.h"
#include "Profile.h"
#include "Stadium.h"
#include "MeshObject.h"
#include "FramebufferRenderer.h"
#include "Timer.h"
#include "StadiumPreview.h"

#include "Units.h"
using namespace Units;

class Beyblade;
class PhysicsWorld;
class StadiumPreview;

// Online: restrict to distinct profiles but otherwise anything can be selected
// 
struct Player {
    std::string name;
    std::vector<Beyblade> beyblades;
    int chosenBeybladeIndex;
};

class SelectionState : public GameState {
public:
    SelectionState(GameEngine* _game) : GameState(_game) {};

    void init() override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvents() override;
    void onResize(int width, int height) override;

    void update(float deltaTime) override;
    void draw() override;

    GameStateType getStateType() const override { return GameStateType::SELECTION; }

private:
    void precomputeLayout(int& leftPanelWidth, int& rightPanelWidth);
    
    void showPlayerButtons();
    void showPlayers();
    void showStadiumOptions();
    void showPhysicsOptions();

    std::vector<std::shared_ptr<Stadium>> stadiums; // TODO: Make profile default stadium?
    std::vector<std::shared_ptr<Beyblade>> players; // TODO: Modify this to support differnt profiles in the future?
    std::shared_ptr<PhysicsWorld> physicsWorld;

    std::unique_ptr<StadiumPreview> stadiumPreview; // Our new unified preview

    float tempRadius = StadiumDefaults::radius;
    glm::vec3 tempCenter = StadiumDefaults::center;
    float tempCurvature = StadiumDefaults::curvature;
    float tempFriction = StadiumDefaults::COF;
    int tempVerticesPerRing = StadiumDefaults::verticesPerRing;
    int tempNumRings = StadiumDefaults::numRings;
    glm::vec3 tempTint = StadiumDefaults::tint;
    glm::vec3 tempRingColor = StadiumDefaults::ringColor;
    glm::vec3 tempCrossColor = StadiumDefaults::crossColor;


    float tempGravity = 0.0f;


    const int MAX_PLAYERS = 8;
};
