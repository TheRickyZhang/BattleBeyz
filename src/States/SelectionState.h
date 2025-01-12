#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "Camera.h"
#include "GameState.h"
#include "Profile.h"
#include "Stadium.h"
#include "MeshObject.h"
#include "FramebufferRenderer.h"

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

    // TODO: Move this elsewhere and see if used?
    glm::mat4 setupCamera(int previewWidth, int previewHeight);

    // For interacting with stadium preview
    std::unique_ptr<FramebufferRenderer> stadiumRenderer;
    std::unique_ptr<Camera> camera = nullptr;
    bool isMouseHoveringPreview = false;

    // TODO: Change magic numbers here
    int previewWidth = 300;
    int previewHeight = 300;
    std::shared_ptr<Stadium> stadium = nullptr; // TODO: Make profile default stadium?

    glm::mat4 previewModelMatrix = glm::mat4(1.0f);

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

    

    const int MAX_PLAYERS = 8;

    // Int slider for step size n != 1
    bool SliderIntDiscrete(const char* label, int* value, int minVal, int maxVal, int step) {
        *value = std::clamp(*value, minVal, maxVal);
        if ((*value - minVal) % step != 0) {
            *value = minVal + ((*value - minVal) / step) * step;
        }
        bool modified = ImGui::SliderInt(label, value, minVal, maxVal, "%d");
        if (modified) {
            *value = minVal + ((*value - minVal + step / 2) / step) * step; // Round to nearest step
        }

        return modified;
    }

};
