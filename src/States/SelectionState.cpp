#include "SelectionState.h"

#include "StateIdentifiers.h"
#include "Beyblade.h"
#include "BeybladeConstants.h"
#include "ProfileManager.h"
#include "PhysicsWorld.h"
#include "UI.h"

using namespace std;
using namespace ImGui;

void SelectionState::init() {

}

void SelectionState::cleanup() {}

void SelectionState::pause() {}

void SelectionState::resume() {}

void SelectionState::handleEvents() {}

void SelectionState::update(float deltaTime) {}

// Draw Method
void SelectionState::draw() {
    int leftPanelWidth, rightPanelWidth;
    precomputeLayout(leftPanelWidth, rightPanelWidth);

    SetWindowPositionAndSize(1, 1, 1, 1);

    Begin("Select Menu");

    if (Button("Back to Home")) {
        game->changeState(GameStateType::HOME);
        End();
        return;
    }

    // Left and right panels
    drawPlayerPanels(game->pm.getAllProfiles(), leftPanelWidth, rightPanelWidth); // Replace with actual logic to fetch profiles

    // Draw "Play" button
    if (Button("Play")) {
        game->changeState(GameStateType::ACTIVE); // TODO: Pass parameters here
        End();
        return;
    }

    End();
}


void SelectionState::precomputeLayout(int& leftPanelWidth, int& rightPanelWidth) {
    // Compute the layout widths
    int windowWidth = game->windowWidth;
    rightPanelWidth = static_cast<int>(windowWidth * 0.25f);
    leftPanelWidth = static_cast<int>(windowWidth * 0.75f);
}

void SelectionState::drawPlayerPanels(const vector<shared_ptr<Profile>>& players, int leftPanelWidth, int rightPanelWidth) {
    int numPlayers = static_cast<int>(players.size());
    if (numPlayers < 1 || numPlayers > 8) {
        // Handle invalid player counts
        Text("Invalid number of players. Must be between 1 and 8.");
        return;
    }

    int windowHeight = game->windowHeight; // Get the window height

    // Left Panel: Players
    BeginChild("LeftPanel", ImVec2(static_cast<float>(leftPanelWidth), static_cast<float>(windowHeight)), true);
    {
        Text("Players");
        Separator();

        // TODO: Implement dynamic layout logic for players (1-8)
        for (int i = 0; i < numPlayers; ++i) {
            Text("Player %d: %s", i + 1, players[i]->getName().c_str());
        }
    }
    EndChild();

    SameLine(); // Ensure the right panel is aligned to the right of the left panel

    // Right Panel: Stadium and Physics
    BeginChild("RightPanel", ImVec2(static_cast<float>(rightPanelWidth), static_cast<float>(windowHeight)), true);
    {
        PhysicsWorld* physicsWorld = game->physicsWorld;
        Text("Stadium and Physics");
        Separator();

        Text("Select Stadium:");

        Separator();
        Text("Physics Parameters:");
        SliderFloat("Gravity", &(this->tempGravity), 0.0f, 20.0f, "Gravity: %.2f");

        if (Button("SetPHysisWorld")) {
  /*          Physics physics = Physics(gravity)
            physicsWorld->setPhysics*/
        }
    }
    EndChild();
}