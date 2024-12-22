#include "CustomizeState.h"
#include "StateIdentifiers.h"
#include "ProfileManager.h"
#include "UI.h"
#include "BeybladeConstants.h"
#include "../lib/ImGuiFileDialog/ImGuiFileDialog.h"
#include "../lib/ImGuiFileDialog/ImGuiFileDialogConfig.h"

using namespace std;
using namespace ImGui;

void CustomizeState::init() {}

void CustomizeState::cleanup() {}

void CustomizeState::pause() {}

void CustomizeState::resume() {}

void CustomizeState::handleEvents() {}

void CustomizeState::update(float deltaTime) {}

// Draw Method
void CustomizeState::draw() {
    // Precompute layout parameters
    float windowWidth, frameSpacing, spacing, leftTextWidth;
    float rightButton1X, rightButton2X, dropdownLeftX, dropdownWidth;
    precomputeLayout(windowWidth, frameSpacing, spacing, leftTextWidth, rightButton1X, rightButton2X, dropdownLeftX, dropdownWidth);

    // Initialize data for rendering
    vector<shared_ptr<Profile>> profiles;
    shared_ptr<Profile> profile = nullptr;
    vector<shared_ptr<Beyblade>> beyblades;
    shared_ptr<Beyblade> beyblade = nullptr;
    initializeData(profiles, profile, beyblades, beyblade);

    // Cover full screen
    SetWindowPositionAndSize(1, 1, 1, 1);

    // Begin main window
    Begin("Customize Menu");

    // Draw "Back to Home" button
    if (Button("Back to Home")) {
        game->changeState(GameStateType::HOME);
        End();  // Ensures proper window closure
        return;
    }

    // Draw Profile Section
    profile = drawProfileSection(profiles, profile, dropdownLeftX, dropdownWidth, rightButton1X, rightButton2X);

    // Draw Beyblade Section if a profile is active
    if (profile) {
        beyblade = drawBeybladeSection(beyblades, beyblade, profile, dropdownLeftX, dropdownWidth, rightButton1X, rightButton2X);
    }
    SeparatorSpacedThick();

    // Draw Customize Section
    if (!profile) {
        Text("No Profiles Found");
    }
    else if (!beyblade) {
        Text("No Beyblades Found");
    }
    else if (beyblade->isTemplate) {
        drawTemplateCustomizeSection(beyblade);
    }
    else {
        drawManualCustomizeSection(beyblade);
    }

    // Handle Popups
    drawPopups(profile, beyblade);

    // End main window
    End();
}

// Precompute layout parameters
void CustomizeState::precomputeLayout(float& windowWidth, float& frameSpacing, float& spacing,
    float& leftTextWidth, float& rightButton1X, float& rightButton2X,
    float& dropdownLeftX, float& dropdownWidth) {
    windowWidth = static_cast<float>(game->windowWidth);
    frameSpacing = GetStyle().FramePadding.x;
    spacing = GetStyle().ItemSpacing.x;
    leftTextWidth = max(CalcTextSize("Profile").x, CalcTextSize("Beyblade").x) + frameSpacing * 2;
    float rightButton2Width = CalcTextSize("Delete##profile").x + frameSpacing * 2;
    float rightButton1Width = CalcTextSize("Create New").x + frameSpacing * 2;
    rightButton1X = windowWidth - rightButton1Width - rightButton2Width - 2 * spacing;
    rightButton2X = windowWidth - rightButton2Width - spacing;
    dropdownLeftX = leftTextWidth + spacing;
    float dropdownRightEdge = rightButton1X - spacing;
    dropdownWidth = dropdownRightEdge - dropdownLeftX;
}

// Initialize data for rendering
// TODO: This data only needs to be recomputed on initialization or change in dropdown selection. However the performance is not very important now
void CustomizeState::initializeData(vector<shared_ptr<Profile>>& profiles, shared_ptr<Profile>& profile,
    vector<shared_ptr<Beyblade>>& beyblades, shared_ptr<Beyblade>& beyblade) {
    profiles = game->pm.getAllProfiles();
    profile = game->pm.getActiveProfile();
    if (profile) {
        beyblades = profile->getAllBeyblades();
        currentProfileName = profile->getName();
        if (!beyblades.empty()) {
            beyblade = profile->getActiveBeyblade();
            currentBeybladeName = beyblade->getName();
        }
        else {
            currentBeybladeName = "No Beyblade Selected";
        }
    }
    else {
        currentProfileName = "No Profile Selected";
    }
}

// Draw Profile Section
std::shared_ptr<Profile> CustomizeState::drawProfileSection(const vector<shared_ptr<Profile>>& profiles, const shared_ptr<Profile>& activeProfile,
    float dropdownLeftX, float dropdownWidth, float rightButton1X, float rightButton2X) {
    // Line 2: [Profile] <{Profile Dropdown}...> [Create New] [Delete]
    Text("Profile");
    SameLine();
    SetCursorPosX(dropdownLeftX);
    SetNextItemWidth(dropdownWidth);
    std::shared_ptr<Profile> updatedProfile = activeProfile;
    if (BeginCombo("##ProfileCombo", currentProfileName.c_str())) {
        for (const auto& profile : profiles) {
            bool isSelected = (activeProfile == profile);
            if (Selectable(profile->getName().c_str(), isSelected)) {
                game->pm.setActiveProfile(profile->getId());
                updatedProfile = profile;
                currentProfileName = profile->getName();
                prevbladeBody = nullptr;  // Refresh beyblade data on next draw
            }
            if (isSelected) {
                SetItemDefaultFocus();
            }
        }
        EndCombo();
    }

    // Create New Profile Button
    SameLine();
    SetCursorPosX(rightButton1X);
    if (Button("Create New##profile")) {
        currentPopup = PopupState::NEW_PROFILE;
        OpenPopup("New Profile");
    }

    // Delete Profile Button
    if (updatedProfile) {
        SameLine();
        SetCursorPosX(rightButton2X);
        if (Button("Delete##profile")) {
            currentPopup = PopupState::DELETE_PROFILE;
            OpenPopup("Confirm Profile Deletion");
        }
    }
    return updatedProfile; // Return update to assign while not controlling global state
}

// Draw Beyblade Section
shared_ptr<Beyblade> CustomizeState::drawBeybladeSection(const vector<shared_ptr<Beyblade>>& beyblades, const shared_ptr<Beyblade>& activeBeyblade,
    const shared_ptr<Profile>& profile, float dropdownLeftX, float dropdownWidth, float rightButton1X, float rightButton2X) {
    // Line 3: [Beyblade] <{Beyblade Dropdown}...> [Create New] [Delete]
    Text("Beyblade");
    SameLine();
    SetCursorPosX(dropdownLeftX);
    SetNextItemWidth(dropdownWidth);
    shared_ptr<Beyblade> updatedBeyblade = activeBeyblade;
    if (BeginCombo("##BeybladeCombo", currentBeybladeName.c_str())) {
        for (const auto& beyblade : beyblades) {
            bool isSelected = (beyblade == activeBeyblade);
            if (Selectable(beyblade->getName().c_str(), isSelected)) {
                updatedBeyblade = beyblade;
                profile->setActiveBeyblade(beyblade->getId());
                currentBeybladeName = beyblade->getName();
            }
            if (isSelected) {
                SetItemDefaultFocus();
            }
        }
        EndCombo();
    }

    // Create New Beyblade Button
    SameLine();
    SetCursorPosX(rightButton1X);
    if (Button("Create New##blade")) {
        currentPopup = PopupState::NEW_BEYBLADE;
        OpenPopup("New Beyblade");
    }

    // Delete Beyblade Button
    if (updatedBeyblade) {
        SameLine();
        SetCursorPosX(rightButton2X);
        if (Button("Delete##blade")) {
            currentPopup = PopupState::DELETE_BEYBLADE;
            OpenPopup("Confirm Beyblade Deletion");
        }
    }
    return updatedBeyblade;
}

// Line 4: Customize Your Beyblade || No Beyblades To Customize || Current Beyblade Not Selected
void CustomizeState::drawManualCustomizeSection(shared_ptr<Beyblade> beyblade) {
    // We assume from prior checks that beybladeBody is never nullptr here
    BeybladeBody* beybladeBody = beyblade->getRigidBody();

    Text("Customize Your Beyblade");
    // Update temporary variables if beyblade has changed
    if (beybladeBody != prevbladeBody) {
        BeybladeParameter::assignFromBeybladeBody(beybladeBody);
        prevbladeBody = beybladeBody;
    }

    auto onModified = [&]() { beybladeBody->setModified(); };

    // Mesh loading code.

    if (Button("Select Custom Mesh")) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.countSelectionMax = 1;
        config.flags = ImGuiFileDialogFlags_Modal;

        ImGuiFileDialog::Instance()->OpenDialog("Dlg##SelectMesh", "Choose File", ".obj", config);
    }
    SeparatorSpaced();

    // This displays the file dialog when the button is pressed.
    //
    // If the minimum size is not specified you can't see the dialog (seems like a bug).
    if (ImGuiFileDialog::Instance()->Display("Dlg##SelectMesh", ImGuiWindowFlags_None, ImVec2(800, 600))) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            //std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            // Load the new mesh.
            //
            // TODO: Need to copy some dimension info??

            auto newMesh = std::make_unique<BeybladeMesh>(filePathName.c_str());  // This loads the mesh.
            if (newMesh != nullptr && newMesh->modelLoaded) {
                beyblade->setMesh(newMesh);
            }
            else {
                game->ml.addMessage("Failed to load mesh. Ensure it is a valid .obj file", MessageType::ERROR, true);
            }
        }

        ImGuiFileDialog::Instance()->Close();  // Close the dialog
        return;
    }

    for (BeybladeParameter& layer : layerParameters) {
        DrawDiscreteFloatControl(layer.name.c_str(), getMaxLayerTextSize(), "layer", layer.currentValue, layer.minValue, layer.maxValue,
            layer.getStepSize(), layer.getFastStepSize(), layer.getDisplayFormat().c_str(), onModified);
    }
    SeparatorSpaced();
    for (BeybladeParameter& disc : discParameters) {
        DrawDiscreteFloatControl(disc.name.c_str(), getMaxDiscTextSize(), "disc", disc.currentValue, disc.minValue, disc.maxValue,
            disc.getStepSize(), disc.getFastStepSize(), disc.getDisplayFormat().c_str(), onModified);
    }
    SeparatorSpaced();
    for (BeybladeParameter& driver : driverParameters) {
        DrawDiscreteFloatControl(driver.name.c_str(), getMaxDriverTextSize(), "driver", driver.currentValue, driver.minValue, driver.maxValue,
            driver.getStepSize(), driver.getFastStepSize(), driver.getDisplayFormat().c_str(), onModified);
    }

    // Line n: Update and Reset Buttons
    SeparatorSpacedThick();

    // Show update button in red if there are active changes
    const ImVec4& buttonModified = ImVec4(0.6f, 0.0f, 0.0f, 1.0f);
    bool modified = beybladeBody->getModified();
    if (modified) PushStyleColor(ImGuiCol_Button, buttonModified);
    if (Button("Update")) {
        BeybladeParameter::assignToBeybladeBody(beybladeBody);
        beybladeBody->setModified(false);
    }
    if (modified) PopStyleColor();

    SameLine();
    float updateButtonWidth = CalcTextSize("Update").x + 2 * GetStyle().ItemSpacing.x + 2 * GetStyle().FramePadding.x;
    SetCursorPosX(GetCursorPosX() + updateButtonWidth);
    if (Button("Reset")) {
        BeybladeParameter::assignFromBeybladeBody(beybladeBody);
        beybladeBody->setModified(false);
    }
}

void CustomizeState::drawTemplateCustomizeSection(shared_ptr<Beyblade> beyblade) {
    Text("Choose your parts (Layer, Disc, Driver)");
    /*
    *  Layer  |  Info
    *  Disc   |  Info
    *  Driver |  Info
    */
    Columns(2, nullptr, true);

    BeginChild("SelectionColumn", ImVec2(0, 0), true);
    Text("Select Parts");
    // **Layer, Disc, Driver Selection**
    if (CollapsingHeader("Layers")) {
        for (int i = 0; i < templateLayers.size(); ++i) {
            bool isSelected = (tempSelectedLayer == i);
            if (Selectable((templateLayers[i].name + "##layer" + to_string(i)).c_str(), isSelected)) {
                tempSelectedLayer = i;
            }
        }
    }
    if (CollapsingHeader("Discs")) {
        for (int i = 0; i < templateDiscs.size(); ++i) {
            bool isSelected = (tempSelectedDisc == i);
            if (Selectable((templateDiscs[i].name + "##disc" + to_string(i)).c_str(), isSelected)) {
                tempSelectedDisc = i;
            }
        }
    }
    if (CollapsingHeader("Drivers")) {
        for (int i = 0; i < (int)templateDrivers.size(); ++i) {
            bool isSelected = (tempSelectedDriver == i);
            if (Selectable((templateDrivers[i].name + "##driver" + to_string(i)).c_str(), isSelected)) {
                tempSelectedDriver = i;
            }
        }
    }
    EndChild();

    NextColumn();
    BeginChild("InfoColumn", ImVec2(0, 0), true);
    Text("Part Details");

    // **Display Layer, Disc, Driver Details**
    bool isLayerSelected = tempSelectedLayer >= 0 && tempSelectedLayer < static_cast<int>(templateLayers.size());
    bool isDiscSelected = tempSelectedDisc >= 0 && tempSelectedDisc < static_cast<int>(templateDiscs.size());
    bool isDriverSelected = tempSelectedDriver >= 0 && tempSelectedDriver < static_cast<int>(templateDrivers.size());
    if (isLayerSelected) {
        const auto& layer = templateLayers[tempSelectedLayer].part;
        Separator();
        Text("Name: %s", templateLayers[tempSelectedLayer].name.c_str());
        Text("Model Path: %s", templateLayers[tempSelectedLayer].modelPath.c_str());
        Text("Mass: %.3f kg", layer.mass);
    }
    if (isDiscSelected) {
        const auto& disc = templateDiscs[tempSelectedDisc].part;
        Separator();
        Text("Name: %s", templateDiscs[tempSelectedDisc].name.c_str());
        Text("Model Path: %s", templateDiscs[tempSelectedDisc].modelPath.c_str());
        Text("Mass: %.3f kg", disc.mass);
    }
    if (isDriverSelected) {
        const auto& driver = templateDrivers[tempSelectedDriver].part;
        Separator();
        Text("Name: %s", templateDrivers[tempSelectedDriver].name.c_str());
        Text("Model Path: %s", templateDrivers[tempSelectedDriver].modelPath.c_str());
        Text("Mass: %.3f kg", driver.mass);
    }

    EndChild();

    // **Update and Reset Buttons**
    Columns(1); // Reset to single column
    if (Button("Update")) {
        // Validate selections
        if (isLayerSelected && isDiscSelected && isDriverSelected) {
            beyblade->update(tempSelectedLayer, tempSelectedDisc, tempSelectedDriver);
            TextColored(ImVec4(0, 1, 0, 1), "Beyblade updated successfully!");
        }
        else {
            TextColored(ImVec4(1, 0, 0, 1), "Invalid selection! Please choose valid parts.");
        }
    }

    SameLine();

    if (Button("Reset")) {
        tempSelectedLayer = beyblade->templateIndices[0];
        tempSelectedDisc = beyblade->templateIndices[1];
        tempSelectedDriver = beyblade->templateIndices[2];
        TextColored(ImVec4(0, 1, 0, 1), "Selections and properties have been reset.");
    }
}

// NOTE: Since drawPopups is the last thing called in draw(), no need to update value of beyblade and profile
void CustomizeState::drawPopups(const shared_ptr<Profile>& profile, const shared_ptr<Beyblade>& beyblade) {
    ProfileManager& pm = game->pm;
    // New Profile Popup
    if (currentPopup == PopupState::NEW_PROFILE && BeginPopupModal("New Profile", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        InputText("##ProfileName", newProfileName, IM_ARRAYSIZE(newProfileName));
        if (Button("Submit")) {
            if (strlen(newProfileName) == 0) {
                game->ml.addMessage("Profile name cannot be empty", MessageType::WARNING, true);
            }
            else if (!pm.createProfile(newProfileName)) {
                game->ml.addMessage("Maximum profile size reached", MessageType::WARNING, true);
            }
            else {
                pm.setActiveProfile(pm.getAllProfiles().back()->getId());
                newProfileName[0] = '\0';
                currentPopup = PopupState::NONE;
                CloseCurrentPopup();
            }
        }

        SameLine();
        if (Button("Cancel")) {
            newProfileName[0] = '\0';
            currentPopup = PopupState::NONE;
            CloseCurrentPopup();
        }
        EndPopup();
    }

    // New Beyblade Popup
    if (currentPopup == PopupState::NEW_BEYBLADE && BeginPopupModal("New Beyblade", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        Text("Choose your type");
        if (RadioButton("Manual", !isTemplate)) {
            isTemplate = false;
        }
        if (RadioButton("Template", isTemplate)) {
            isTemplate = true;
        }

        Text("Enter a name for the new Beyblade:");
        InputText("##BeybladeName", newBeybladeName, IM_ARRAYSIZE(newBeybladeName));
        if (Button("Submit")) {
            if (strlen(newBeybladeName) == 0) {
                game->ml.addMessage("Beyblade name cannot be empty", MessageType::WARNING, true);
            }
            else if (profile) {
                std::string beybladeNameStr(newBeybladeName);
                if (isTemplate) {
                    if (!profile->createBeyblade(beybladeNameStr, true)) {
                        game->ml.addMessage("Cannot create new Beyblade (Template)", MessageType::ERROR, true);
                    }
                }
                else {
                    if (!profile->createBeyblade(beybladeNameStr)) {
                        game->ml.addMessage("Cannot create new Beyblade (Manual)", MessageType::ERROR, true);
                    }
                }
                profile->setActiveBeyblade(profile->getAllBeyblades().back()->getId());
                newBeybladeName[0] = '\0';
                currentPopup = PopupState::NONE;
                CloseCurrentPopup();
            }
        }
        SameLine();
        if (Button("Cancel")) {
            newBeybladeName[0] = '\0';
            currentPopup = PopupState::NONE;
            CloseCurrentPopup();
        }
        EndPopup();
    }

    // Confirm Profile Deletion Popup
    if (currentPopup == PopupState::DELETE_PROFILE && BeginPopupModal("Confirm Profile Deletion", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        Text("Are you sure you want to delete the profile: %s", currentProfileName.c_str());
        if (Button("Yes")) {
            if (!pm.deleteProfile(profile->getId())) {
                game->ml.addMessage("Error Removing Profile (You must have at least one profile)", MessageType::ERROR, true);
            }
            currentPopup = PopupState::NONE;
            CloseCurrentPopup();
        }
        SameLine();
        if (Button("No")) {
            currentPopup = PopupState::NONE;
            CloseCurrentPopup();
        }
        EndPopup();
    }

    // Confirm Beyblade Deletion Popup
    if (currentPopup == PopupState::DELETE_BEYBLADE && BeginPopupModal("Confirm Beyblade Deletion", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        Text("Are you sure you want to delete the Beyblade: %s", currentBeybladeName.c_str());
        if (Button("Yes")) {
            if (!profile->deleteBeyblade(beyblade->getId())) {
                game->ml.addMessage("Error Deleting Beyblade", MessageType::ERROR, true);
            }
            currentPopup = PopupState::NONE;
            CloseCurrentPopup();
        }
        SameLine();
        if (Button("No")) {
            currentPopup = PopupState::NONE;
            CloseCurrentPopup();
        }
        EndPopup();
    }
}