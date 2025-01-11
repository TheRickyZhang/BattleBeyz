#include "SelectionState.h"

#include "StateIdentifiers.h"
#include "Beyblade.h"
#include "BeybladeConstants.h"
#include "ProfileManager.h"
#include "PhysicsWorld.h"
#include "UI.h"
#include "ObjectShader.h"

#include <glm/gtc/type_ptr.hpp>


using namespace std;
using namespace glm;
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

    int windowHeight = game->windowHeight;
     //Left Panel: Players
    BeginChild("LeftPanel", ImVec2(static_cast<float>(leftPanelWidth), static_cast<float>(windowHeight)), true);
    {
        showPlayerButtons();
        showPlayers();
        
    }
    EndChild();

    SameLine();

    BeginChild("RightPanel", ImVec2(static_cast<float>(rightPanelWidth), static_cast<float>(windowHeight)), true);
    {
        showStadiumOptions();

        Separator();

        showPhysicsOptions();
    }
    EndChild();

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


void SelectionState::showPlayerButtons() {
    if (Button("Add player")) {
        if (players.size() >= MAX_PLAYERS) {
            game->ml.addMessage(string("Max player count of") + to_string(MAX_PLAYERS) + " reached!", MessageType::WARNING);
            return;
        }
        // Only show beys from the current profile
        vector<shared_ptr<Beyblade>> currentBeyblades = game->pm.getActiveProfile()->getAllBeyblades();

        OpenPopup("Select Player");
    }
    if (BeginPopup ("Select Player")) {
        Text("Select Player");
        SeparatorSpacedThick();
        vector<shared_ptr<Beyblade>> currentBeyblades = game->pm.getActiveProfile()->getAllBeyblades();
        for (int i = 0; i < static_cast<int>(currentBeyblades.size()); ++i) {
            if (Selectable(currentBeyblades[i]->getName().c_str())) {
                players.push_back(currentBeyblades[i]);
                CloseCurrentPopup();
            }
        }
        EndPopup();
    }
    SameLine();

    if (Button("Remove a player")) {
        if (players.size() <= 0) {
            game->ml.addMessage("No players to remove! Please add a player.", MessageType::WARNING);
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

    int numPlayers = static_cast<int>(players.size());
    int numColumns = std::min(numPlayers, 4);
    int numRows = numPlayers > 4 ? 2 : 1;
    Columns(numColumns, nullptr, false);

    //cout << numColumns << endl;
    //Text("Test 1");
    //NextColumn();
    //Text("Test 2");
    for (int i = 0; i < numPlayers; ++i) {
        const shared_ptr<Beyblade>& beyblade = players[i];
        Text("Name: %s", beyblade->getName().c_str());
        NextColumn();
        // Transition to next column at MOD numColumns (note 0-indexed), except for last one
        if ((i + 1) % numColumns == 0 && i + 1 < numPlayers) {
            Columns(numColumns, nullptr, false);
        }
    }
    Columns(1); // Reset to single column
}


void SelectionState::showStadiumOptions() {
    PhysicsWorld* physicsWorld = game->physicsWorld;
    Text("Stadium and Physics");
    Separator();

    Text("Select Stadium:");

    // Be able to customize stadium
    if (stadium == nullptr) {
        Text("Stadium Body COntrols");
        Separator();
        
        // Stadium Body (Note that center is always set to (0, 0, 0) by default)
        SliderFloat("Radius (m)", &tempRadius, 0.25f, 3.0f);
        SliderFloat("Curvature", &tempCurvature, 0.0f, 0.9f);
        SliderFloat("Friction", &tempFriction, 0.0f, 0.9f);

        // Stadium Mesh
        SliderInt("Roundness (vertices per ring)", &tempVerticesPerRing, 8, 200);
        SliderInt("Number of Rings", &tempNumberOfRings, 4, 100);
        SliderFloat3("Overall Color", glm::value_ptr(tempColor), 0.0f, 1.0f, "Color: %.2f");
        SliderFloat3("Ring Color", glm::value_ptr(tempRingColor), 0.0f, 1.0f, "Color: %.2f");
        SliderFloat3("Cross Color", glm::value_ptr(tempCrossColor), 0.0f, 1.0f, "Color: %.2f");


        // TODO: Upload for texture and texture scale

        if (Button("Set Stadium")) {
            StadiumBody* stadiumBody = new StadiumBody(vec3(0.0f), tempRadius, tempCurvature, tempFriction);
            StadiumMesh* stadiumMesh = new StadiumMesh(nullptr, tempVerticesPerRing, tempNumberOfRings, tempRingColor, tempCrossColor, tempColor, 1.0f);
            stadium = make_shared<Stadium>(stadiumBody, stadiumMesh, "default");

            //renderStadiumPreview(); // TODO: Check if anything is needed here
        }
    }
    else {
        Text("Stadium is selected. Yay!!!");

        renderStadiumPreview();
    }
}

void SelectionState::showPhysicsOptions() {
    Text("Physics Parameters:");
    SliderFloat("Gravity", &(this->tempGravity), 0.0f, 20.0f, "Gravity: %.2f");

    if (Button("SetPhysicsWorld")) {
        /*          Physics physics = Physics(gravity)
                  physicsWorld->setPhysics*/
    }
}

void SelectionState::setupStadiumPreview() {
    
}

void SelectionState::renderStadiumPreview() {
    // TODO: Make these adaptable to screen size
    int previewX = 10, previewY = 10;
    int previewWidth = 300, previewHeight = 300;

    setupViewport(previewX, previewY, previewWidth, previewHeight);


    glm::mat4 projection = setupCamera(previewWidth, previewHeight);
    glm::mat4 view = glm::lookAt(
        glm::vec3(5.0f, 5.0f, 5.0f), // Camera pos
        glm::vec3(0.0f, 0.0f, 0.0f), // Center of stadium 
        glm::vec3(5.0f, 1.0f, 0.0f) // Up vector
    );

    game->objectShader->use();
    ObjectShader* objectShader = game->objectShader;
    objectShader->setMat4("projection", projection);
    objectShader->setMat4("view", view);

    if (stadium) stadium->render(*objectShader);

    resetViewport();
   
}

void SelectionState::setupViewport(int previewX, int previewY, int previewWidth, int previewHeight) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(previewX, previewY, previewWidth, previewHeight);
    glViewport(previewX, previewY, previewWidth, previewHeight);
}

void SelectionState::resetViewport() {
    glDisable(GL_SCISSOR_TEST);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

glm::mat4 SelectionState::setupCamera(int previewWidth, int previewHeight) {
    return glm::perspective(
        glm::radians(45.0f),
        (float)previewWidth / previewHeight,
        0.1f,
        100.0f
    );
}