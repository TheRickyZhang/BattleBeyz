#include "SelectionState.h"

#include "Beyblade.h"
#include "BeybladeConstants.h"
#include "InputUtils.h"
#include "ObjectShader.h"
#include "ProfileManager.h"
#include "PhysicsWorld.h"
#include "StateIdentifiers.h"
#include "UI.h"

#include <glm/gtc/type_ptr.hpp>


using namespace std;
using namespace glm;
using namespace ImGui;

void SelectionState::init() {
    stadiumRenderer = make_unique<FramebufferRenderer>(previewWidth, previewHeight);
    camera = make_unique<Camera>(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f), game->physicsWorld, 
        static_cast<float>(previewWidth), static_cast<float>(previewHeight));
}

void SelectionState::cleanup() {}

void SelectionState::pause() {}

void SelectionState::resume() {}

void SelectionState::handleEvents() {
    if (isMouseHoveringPreview) {
        if (IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 mouseDelta = GetMouseDragDelta(ImGuiMouseButton_Left);
            camera->processMouseMovement(mouseDelta.x, -mouseDelta.y);  // Negative y for inverted controls
            ResetMouseDragDelta(ImGuiMouseButton_Left);
        }
        if (GetIO().MouseWheel != 0.0f) {
            camera->processMouseScroll(GetIO().MouseWheel);
        }
    }

    //cout << "Game dt" << game->deltaTime << endl;
    // Handle keyboard inputs for the camera
    if (IsKeyPressed(ImGuiKey_W)) camera->processKeyboard(Action::MoveForward, game->deltaTime);
    if (IsKeyPressed(ImGuiKey_S)) camera->processKeyboard(Action::MoveBackward, game->deltaTime);
    if (IsKeyPressed(ImGuiKey_A)) camera->processKeyboard(Action::MoveLeft, game->deltaTime);
    if (IsKeyPressed(ImGuiKey_D)) camera->processKeyboard(Action::MoveRight, game->deltaTime);
}

void SelectionState::update(float deltaTime) {
    camera->update(deltaTime);
}

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

    Text("Modify Stadium");
    Separator();

    if (stadium == nullptr) {
        Text("No stadium selected!");
    }
    else {
        renderStadiumPreview();
        ImGui::BeginChild("StadiumPreview", ImVec2(static_cast<float>(previewWidth), static_cast<float>(previewHeight)), false, ImGuiWindowFlags_NoScrollbar);
        {
            isMouseHoveringPreview = IsItemHovered(); // Check if the mouse is over the preview
            Image((void*)(intptr_t)stadiumRenderer->getTexture(), ImVec2(static_cast<float>(previewWidth), static_cast<float>(previewHeight)));
        }
        ImGui::EndChild();
    }
    
    StadiumBody& body = *stadium->getRigidBody();
    StadiumMesh& mesh = *stadium->getMesh();

    // Stadium Body (Note that center is always set to (0, 0, 0) by default)
    if (SliderFloat("Radius (m)", &tempRadius, 0.25f, 3.0f)) {
        body.setRadius(tempRadius);
    }
    if (SliderFloat("Curvature", &tempCurvature, 0.0f, 0.9f)) {
        body.setCurvature(tempCurvature);
    }
    if (SliderFloat("Friction", &tempFriction, 0.0f, 0.9f)) {
        body.setFriction(tempFriction);
    }

    // Stadium Mesh
    //if (SliderIntDiscrete("Vertices per Ring", &tempVerticesPerRing, 8, 200, 4)) {
    //    mesh.setVerticesPerRing(tempVerticesPerRing);
    //}
    //if (SliderIntDiscrete("Number of Rings", &tempNumberOfRings, 4, 100, 4)) {
    //    mesh.setNumberOfRings(tempNumberOfRings);
    //}
    //if (SliderFloat3("Overall Color", glm::value_ptr(tempColor), 0.0f, 1.0f, "Color: %.2f")) {
    //    mesh.setOverallColor(tempColor);
    //}
    //if (SliderFloat3("Ring Color", glm::value_ptr(tempRingColor), 0.0f, 1.0f, "Color: %.2f")) {
    //    mesh.setRingColor(tempRingColor);
    //}
    //if (SliderFloat3("Cross Color", glm::value_ptr(tempCrossColor), 0.0f, 1.0f, "Color: %.2f")) {
    //    mesh.setCrossColor(tempCrossColor);
    //}


    // TODO: Upload for texture and texture scale

    if (Button("Set Stadium")) {
        unique_ptr<StadiumBody> stadiumBody = make_unique<StadiumBody>(vec3(0.0f), tempRadius, tempCurvature, tempFriction);
        unique_ptr<StadiumMesh> stadiumMesh = make_unique<StadiumMesh>(nullptr, tempVerticesPerRing, tempNumberOfRings, tempRingColor, tempCrossColor, tempColor, 1.0f);
        stadium = make_shared<Stadium>(move(stadiumBody), move(stadiumMesh), "default");
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
    if (stadium->getMesh() == nullptr) {
        std::cerr << "Error: StadiumMesh is nullptr" << std::endl;
        return;
    }
    stadiumRenderer->bind();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ObjectShader* objectShader = game->objectShader;
    objectShader->use();
    objectShader->setMat4("projection", glm::perspective(glm::radians(camera->zoom), (float)previewWidth / previewHeight, 0.1f, 100.0f));
    objectShader->setMat4("view", camera->getViewMatrix());

    stadium->render(*objectShader);

    stadiumRenderer->unbind();
}

glm::mat4 SelectionState::setupCamera(int previewWidth, int previewHeight) {
    return glm::perspective(
        glm::radians(45.0f),
        (float)previewWidth / previewHeight,
        0.1f,
        100.0f
    );
}