#include "SelectionState.h"

#include "StateIdentifiers.h"
#include "Beyblade.h"
#include "BeybladeConstants.h"
#include "ProfileManager.h"
#include "UI.h"
#include "../lib/ImGuiFileDialog/ImGuiFileDialog.h"
#include "../lib/ImGuiFileDialog/ImGuiFileDialogConfig.h"

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
    // Cover full screen
    SetWindowPositionAndSize(1, 1, 1, 1);

    // Begin main window
    Begin("Select Menu");

    // Draw "Back to Home" button
    if (Button("Back to Home")) {
        game->changeState(GameStateType::HOME);
        End();
        return;
    }

    //// Draw Beyblade Section if a profile is active
    //if (profile) {
    //    beyblade = drawBeybladeSection(beyblades, beyblade, profile, dropdownLeftX, dropdownWidth, rightButton1X, rightButton2X);
    //}
    //SeparatorSpacedThick();

    //// Draw Customize Section
    //if (!profile) {
    //    Text("No Profiles Found");
    //}
    //else if (!beyblade) {
    //    Text("No Beyblades Found. Cannot start a battle!");
    //}
    //vector<Beyblade> beyblades = game->pm.getActiveProfile()->getAllBeyblades();


    // End main window
    End();
}
