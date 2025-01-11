#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "GameState.h"
#include "Profile.h"
#include "Stadium.h"
#include "MeshObject.h"

#include "Units.h"
using namespace Units;

class Beyblade;
class PhysicsWorld;

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
    void update(float deltaTime) override;
    void draw() override;

    GameStateType getStateType() const override { return GameStateType::SELECTION; }

private:
    void precomputeLayout(int& leftPanelWidth, int& rightPanelWidth);
    std::vector<std::shared_ptr<Beyblade>> players; // TODO: Modify this to support differnt profiles in the future?
    
    void showPlayerButtons();
    void showPlayers();
    void showStadiumOptions();
    void showPhysicsOptions();

    void setupStadiumPreview();
    void renderStadiumPreview();

    void setupViewport(int previewX, int previewY, int previewWidth, int previewHeight);
    void resetViewport();
    glm::mat4 setupCamera(int previewWidth, int previewHeight);

    float tempGravity = 0.0f;

    // Align these with usual defaults
    float tempRadius = 1.2f;
    float tempCurvature = 0.1f;
    float tempFriction = 0.35f;

    int tempVerticesPerRing = 1;
    int tempNumberOfRings = 1;
    glm::vec3 tempColor = glm::vec3(1.0f);
    glm::vec3 tempRingColor = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 tempCrossColor = glm::vec3(0.0f, 0.0f, 1.0f);

    std::shared_ptr<Stadium> stadium = nullptr; // TODO: Make profile default stadium?

    std::unique_ptr<MeshObject> stadiumPreview = nullptr;
    glm::mat4 previewModelMatrix;
    

    const int MAX_PLAYERS = 8;
};
