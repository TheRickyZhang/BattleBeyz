#include "SelectionState.h"

#include "DefaultValues.h"
#include "Beyblade.h"
#include "BeybladeConstants.h"
#include "InputUtils.h"
#include "ObjectShader.h"
#include "ProfileManager.h"
#include "PhysicsWorld.h"
#include "StateIdentifiers.h"
#include "ImGuiUI.h"
#include "ActiveState.h"

#include <glm/gtc/type_ptr.hpp>


using namespace std;
using namespace glm;
using namespace ImGui;

void SelectionState::init() {
    physicsWorld = make_shared<PhysicsWorld>();

    auto defaultStadium = make_shared<Stadium>();
    stadiums.push_back(defaultStadium);

    // Create our new StadiumPreview
    stadiumPreview = make_unique<StadiumPreview>(300, 300, physicsWorld.get(), game->objectShader);
}

void SelectionState::cleanup() {
    glViewport(0, 0, game->windowWidth, game->windowHeight);
}

void SelectionState::pause() {}
void SelectionState::resume() {}

void SelectionState::handleEvents() {
    // Let the StadiumPreview handle user input (camera rotation, zoom) only if hovered
    stadiumPreview->handleInput(game->deltaTime);
}

void SelectionState::onResize(int width, int height) {
    // If you want your preview to adapt to a resized main window, handle that here
}

void SelectionState::update(float deltaTime) {
    stadiumPreview->update(deltaTime, game->currTime);
}

void SelectionState::draw() {
    int leftPanelWidth, rightPanelWidth;
    precomputeLayout(leftPanelWidth, rightPanelWidth);

    SetWindowPositionAndSize(1, 1, 1, 1);
    Begin("Select Menu");

    if (Button("Back to Home")) {
        game->changeState(StateFactory::createState(game, GameStateType::HOME));
        End();
        return;
    }

    int windowHeight = game->windowHeight;
    BeginChild("LeftPanel", ImVec2((float)leftPanelWidth, (float)windowHeight), true);
    {
        showPlayerButtons();
        showPlayers();
    }
    EndChild();

    SameLine();

    BeginChild("RightPanel", ImVec2((float)rightPanelWidth, (float)windowHeight), true);
    {
        showStadiumOptions();
        Separator();
        showPhysicsOptions();

        if (Button("Play")) {
            EndChild();
            End();
            game->changeState(make_unique<ActiveState>(game, stadiums, players, physicsWorld));
            return;
        }
    }
    EndChild();

    End();
}

void SelectionState::precomputeLayout(int& leftPanelWidth, int& rightPanelWidth) {
    int windowWidth = game->windowWidth;
    rightPanelWidth = (int)(windowWidth * 0.25f);
    leftPanelWidth = (int)(windowWidth * 0.75f);
}

void SelectionState::showPlayerButtons() {
    if (Button("Add player")) {
        if (players.size() >= MAX_PLAYERS) {
            game->ml.addMessage("Max player count reached!", MessageType::WARNING);
            return;
        }
        // Show available Beyblades from the active profile
        vector<shared_ptr<Beyblade>> currentBeyblades = game->pm.getActiveProfile()->getAllBeyblades();
        OpenPopup("Select Player");
    }
    if (BeginPopup("Select Player")) {
        Text("Select Player");
        SeparatorSpacedThick();
        vector<shared_ptr<Beyblade>> currentBeyblades = game->pm.getActiveProfile()->getAllBeyblades();
        for (int i = 0; i < (int)currentBeyblades.size(); ++i) {
            if (Selectable(currentBeyblades[i]->getName().c_str())) {
                players.push_back(currentBeyblades[i]);
                CloseCurrentPopup();
            }
        }
        EndPopup();
    }

    SameLine();

    if (Button("Remove a player")) {
        if (players.empty()) {
            game->ml.addMessage("No players to remove!", MessageType::WARNING);
            return;
        }
        OpenPopup("Remove Player");
    }
    if (BeginPopup("Remove Player")) {
        Text("Select a player to remove");
        Separator();
        for (size_t i = 0; i < players.size(); ++i) {
            if (Selectable(players[i]->getName().c_str())) {
                players.erase(players.begin() + i);
                CloseCurrentPopup();
            }
        }
        EndPopup();
    }
}

void SelectionState::showPlayers() {
    Text("Players:");
    Separator();

    if (players.empty()) {
        Text("No players! Please add players.");
        return;
    }

    int numPlayers = (int)players.size();
    int numColumns = std::min(numPlayers, 4);
    Columns(numColumns, nullptr, false);

    for (int i = 0; i < numPlayers; ++i) {
        Text("Name: %s", players[i]->getName().c_str());
        NextColumn();
    }
    Columns(1);
}

void SelectionState::showStadiumOptions() {
    Text("Stadium and Physics");
    Separator();

    if (stadiums.empty()) {
        Text("No stadium selected!");
        return;
    }

    // Draw the stadium preview
    stadiumPreview->draw();

    // Access the stadium from the preview
    Stadium* st = stadiumPreview->getStadium();
    if (!st) return; // Safety

    // Sliders to modify the stadium in real time
    if (SliderFloat("Radius (m)", &tempRadius, StadiumDefaults::radiusMin, StadiumDefaults::radiusMax)) {
        st->setRadius(tempRadius);
    }
    if (SliderFloat3("Center (x,y,z)", glm::value_ptr(tempCenter),
        *glm::value_ptr(StadiumDefaults::centerMin),
        *glm::value_ptr(StadiumDefaults::centerMax))) {
        st->setCenter(tempCenter);
    }
    if (SliderFloat("Curvature", &tempCurvature, StadiumDefaults::curvatureMin, StadiumDefaults::curvatureMax)) {
        st->setCurvature(tempCurvature);
    }
    if (SliderFloat("Friction", &tempFriction, StadiumDefaults::COFMin, StadiumDefaults::COFMax)) {
        st->setFriction(tempFriction);
    }
    if (SliderIntDiscrete("Vertices per Ring", &tempVerticesPerRing,
        StadiumDefaults::verticesPerRingMin,
        StadiumDefaults::verticesPerRingMax,
        4))
    {
        st->setVerticesPerRing(tempVerticesPerRing);
    }
    if (SliderIntDiscrete("Number of Rings", &tempNumRings,
        StadiumDefaults::numRingsMin,
        StadiumDefaults::numRingsMax,
        4))
    {
        st->setNumRings(tempNumRings);
    }
    if (SliderFloat3("Overall Color", glm::value_ptr(tempTint), 0.f, 1.f, "Color: %.2f")) {
        st->setTint(tempTint);
    }
    if (SliderFloat3("Ring Color", glm::value_ptr(tempRingColor), 0.f, 1.f, "Color: %.2f")) {
        st->setRingColor(tempRingColor);
    }
    if (SliderFloat3("Cross Color", glm::value_ptr(tempCrossColor), 0.f, 1.f, "Color: %.2f")) {
        st->setCrossColor(tempCrossColor);
    }

    // Example "Apply" or "Revert" logic
    if (Button("Update Stadium")) {
        *stadiums[0].get() = *st;
    }
    SameLine();
    if (Button("Undo Changes")) {
        // Copy values from stadiums[0] back to the preview stadium
        auto st = stadiumPreview->getStadium();
        *st = *stadiums[0];

        // Sync sliders with the reverted stadium values
        tempRadius = stadiums[0]->getRadius().value();
        tempCenter = stadiums[0]->getCenter().value();
        tempCurvature = stadiums[0]->getCurvature().value();
        tempFriction = stadiums[0]->getCOF().value();
        tempVerticesPerRing = stadiums[0]->getVerticesPerRing();
        tempNumRings = stadiums[0]->getNumRings();
        tempTint = stadiums[0]->getTint();
        tempRingColor = stadiums[0]->getRingColor();
        tempCrossColor = stadiums[0]->getCrossColor();
        stadiumPreview->draw();
    }
}

void SelectionState::showPhysicsOptions() {
    Text("Physics Parameters:");
    SliderFloat("Gravity", &tempGravity, 0.0f, 20.0f, "Gravity: %.2f");
    if (Button("SetPhysicsWorld")) {
        // Example usage:
        // physicsWorld->setGravity(tempGravity);
    }
}
