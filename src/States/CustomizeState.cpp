#include "CustomizeState.h"
#include "StateIdentifiers.h"
#include "ProfileManager.h"
#include "UI.h"

using namespace std;
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
    std::vector<std::shared_ptr<Profile>> profiles;
    std::vector<std::shared_ptr<Beyblade>> beyblades;
    shared_ptr<Beyblade> beyblade = nullptr;
    initializeData(profiles, beyblades, beyblade);

    // Cover full screen
    SetWindowPositionAndSize(1, 1, 1, 1);

    // Begin main window
    ImGui::Begin("Customize Menu");

    // Draw "Back to Home" button
    if (ImGui::Button("Back to Home")) {
        game->changeState(GameStateType::HOME);
        ImGui::End();  // Ensures proper window closure
        return;
    }

    // Draw Profile Section
    drawProfileSection(profiles, dropdownLeftX, dropdownWidth, rightButton1X, rightButton2X);

    // Draw Beyblade Section if a profile is active
    if (game->pm.getActiveProfile()) {
        drawBeybladeSection(beyblades, dropdownLeftX, dropdownWidth, rightButton1X, rightButton2X);
    }

    // Draw Customize Section
    if (!beyblade->getRigidBody()) {
        if (game->pm.getActiveProfile()) {
            ImGui::Text("No Beyblades to Customize");
        }
        else {
            ImGui::Text("Current Beyblade not selected");
        }
    }
    else if (beyblade->isTemplate) {
        drawTemplateCustomizeSection(beyblade);
    }
    else {
        drawManualCustomizeSection(beyblade);
    }

    // Handle Popups
    drawPopups();

    // End main window
    ImGui::End();
}

// Precompute layout parameters
void CustomizeState::precomputeLayout(float& windowWidth, float& frameSpacing, float& spacing,
    float& leftTextWidth, float& rightButton1X, float& rightButton2X,
    float& dropdownLeftX, float& dropdownWidth) {
    windowWidth = static_cast<float>(game->windowWidth);
    frameSpacing = ImGui::GetStyle().FramePadding.x;
    spacing = ImGui::GetStyle().ItemSpacing.x;
    leftTextWidth = std::max(ImGui::CalcTextSize("Profile").x, ImGui::CalcTextSize("Beyblade").x) + frameSpacing * 2;
    float rightButton2Width = ImGui::CalcTextSize("Delete##profile").x + frameSpacing * 2;
    float rightButton1Width = ImGui::CalcTextSize("Create New").x + frameSpacing * 2;
    rightButton1X = windowWidth - rightButton1Width - rightButton2Width - 2 * spacing;
    rightButton2X = windowWidth - rightButton2Width - spacing;
    dropdownLeftX = leftTextWidth + spacing;
    float dropdownRightEdge = rightButton1X - spacing;
    dropdownWidth = dropdownRightEdge - dropdownLeftX;
}

// Initialize data for rendering
// TODO: This data only needs to be recomputed on initialization or change in dropdown selection. However the performance is not very important now
void CustomizeState::initializeData(std::vector<std::shared_ptr<Profile>>& profiles,
    std::vector<std::shared_ptr<Beyblade>>& beyblades,
    shared_ptr<Beyblade>& beyblade) {
    profiles = game->pm.getAllProfiles();
    auto activeProfile = game->pm.getActiveProfile();
    if (activeProfile) {
        beyblades = activeProfile->getAllBeyblades();
        currentProfileName = activeProfile->getName();
        if (!beyblades.empty()) {
            beyblade = activeProfile->getActiveBeyblade();
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
void CustomizeState::drawProfileSection(const std::vector<std::shared_ptr<Profile>>& profiles,
    float dropdownLeftX, float dropdownWidth, float rightButton1X, float rightButton2X) {
    // Line 2: [Profile] <{Profile Dropdown}...> [Create New] [Delete]
    ImGui::Text("Profile");
    ImGui::SameLine();
    ImGui::SetCursorPosX(dropdownLeftX);
    ImGui::SetNextItemWidth(dropdownWidth);
    if (ImGui::BeginCombo("##ProfileCombo", currentProfileName.c_str())) {
        for (const auto& profile : profiles) {
            bool isSelected = (game->pm.getActiveProfile() == profile);
            if (ImGui::Selectable(profile->getName().c_str(), isSelected)) {
                game->pm.setActiveProfile(profile->getId());
                currentProfileName = profile->getName();
                prevbladeBody = nullptr;  // Refresh beyblade data on next draw
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Create New Profile Button
    ImGui::SameLine();
    ImGui::SetCursorPosX(rightButton1X);
    if (ImGui::Button("Create New##profile")) {
        currentPopup = PopupState::NEW_PROFILE;
        ImGui::OpenPopup("New Profile");
    }

    // Delete Profile Button
    ImGui::SameLine();
    ImGui::SetCursorPosX(rightButton2X);
    if (ImGui::Button("Delete##profile")) {
        currentPopup = PopupState::DELETE_PROFILE;
        ImGui::OpenPopup("Confirm Profile Deletion");
    }
}

// Draw Beyblade Section
void CustomizeState::drawBeybladeSection(const std::vector<std::shared_ptr<Beyblade>>& beyblades,
    float dropdownLeftX, float dropdownWidth,
    float rightButton1X, float rightButton2X) {
    // Line 3: [Beyblade] <{Beyblade Dropdown}...> [Create New] [Delete]
    ImGui::Text("Beyblade");
    ImGui::SameLine();
    ImGui::SetCursorPosX(dropdownLeftX);
    ImGui::SetNextItemWidth(dropdownWidth);
    if (ImGui::BeginCombo("##BeybladeCombo", currentBeybladeName.c_str())) {
        for (const auto& beyblade : beyblades) {
            bool isSelected = (beyblade == game->pm.getActiveProfile()->getActiveBeyblade());
            if (ImGui::Selectable(beyblade->getName().c_str(), isSelected)) {
                game->pm.getActiveProfile()->setActiveBeyblade(beyblade->getId());
                currentBeybladeName = beyblade->getName();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Create New Beyblade Button
    ImGui::SameLine();
    ImGui::SetCursorPosX(rightButton1X);
    if (ImGui::Button("Create New##blade")) {
        currentPopup = PopupState::NEW_BEYBLADE;
        ImGui::OpenPopup("New Beyblade");
    }

    // Delete Beyblade Button
    ImGui::SameLine();
    ImGui::SetCursorPosX(rightButton2X);
    if (ImGui::Button("Delete##blade")) {
        currentPopup = PopupState::DELETE_BEYBLADE;
        ImGui::OpenPopup("Confirm Beyblade Deletion");
    }
}

// Line 4: Customize Your Beyblade || No Beyblades To Customize || Current Beyblade Not Selected
void CustomizeState::drawManualCustomizeSection(shared_ptr<Beyblade> beyblade) {
    BeybladeBody* beybladeBody = beyblade->getRigidBody();

    ImGui::Text("Customize Your Beyblade");
    // Update temporary variables if beyblade has changed
    if (beybladeBody != prevbladeBody) {
        tempLayerMass = static_cast<float>(beybladeBody->getLayerMass());
        tempLayerMOI = static_cast<float>(beybladeBody->getLayerMomentOfInertia());
        tempDriverCOF = static_cast<float>(beybladeBody->getDriverCOF());
        prevbladeBody = beybladeBody;
    }

    // Line 5-(n-1): Sliders for Adjusting Variables
    //
    // 2024-12-03 Catch when value changes

    if (ImGui::SliderFloat("Layer Mass (kg)", &tempLayerMass, 0.010f, 0.099f, "%.3f")) {
        if (beybladeBody) {
            beybladeBody->setModified();
        }
    };
    if (ImGui::SliderFloat("Layer Moment of Inertia (kg/m**2)", &tempLayerMOI, 0.000003f, 0.000016f, "%.9f")) {
        if (beybladeBody) {
            beybladeBody->setModified();
        }
    }
    if (ImGui::SliderFloat("Driver Coefficient of Friction", &tempDriverCOF, 0.1f, 0.6f, "%.3f")) {
        if (beybladeBody) {
            beybladeBody->setModified();
        }
    }

    // Line n: Update and Reset Buttons

    const ImVec4& buttonModified = ImVec4(0.6f, 0.0f, 0.0f, 1.0f);  // 2024-12-03 Show Update button in red if blade has been changed
    bool modified = beybladeBody->getModified();
    if (modified) {
        ImGui::PushStyleColor(ImGuiCol_Button, buttonModified);
    }
    if (ImGui::Button("Update")) {
        if (beybladeBody != nullptr) {
            beybladeBody->setLayerMass(tempLayerMass);
            beybladeBody->setMass(tempLayerMass + beybladeBody->getDiscMass() + beybladeBody->getDriverMass());
            beybladeBody->setLayerMomentOfInertia(tempLayerMOI);
            beybladeBody->setDriverCOF(tempDriverCOF);
            beybladeBody->setModified(false);
        }
    }
    if (modified) {  // 2024-12-03  Restore colors
        ImGui::PopStyleColor();
    }

    ImGui::SameLine();
    float updateButtonWidth = ImGui::CalcTextSize("Update").x + 2 * ImGui::GetStyle().ItemSpacing.x + 2 * ImGui::GetStyle().FramePadding.x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + updateButtonWidth);
    if (ImGui::Button("Reset")) {
        if (beybladeBody != nullptr) {
            tempLayerMass = static_cast<float>(beybladeBody->getLayerMass());
            tempLayerMOI = static_cast<float>(beybladeBody->getLayerMomentOfInertia());
            tempDriverCOF = static_cast<float>(beybladeBody->getDriverCOF());
            beybladeBody->setModified(false);  // 2024-12-03
        }
    }
}

void CustomizeState::drawTemplateCustomizeSection(shared_ptr<Beyblade> beyblade) {
    ImGui::Text("Choose your parts (Layer, Disc, Driver)");
    /*
    *  Layer  |  Info
    *  Disc   |  Info
    *  Driver |  Info
    */
    ImGui::Columns(2, nullptr, true);

    ImGui::BeginChild("SelectionColumn", ImVec2(0, 0), true);
    ImGui::Text("Select Parts");
    // **Layer, Disc, Driver Selection**
    if (ImGui::CollapsingHeader("Layers")) {
        for (int i = 0; i < templateLayers.size(); ++i) {
            bool isSelected = (tempSelectedLayer == i);
            if (ImGui::Selectable((templateLayers[i].name + "##layer" + std::to_string(i)).c_str(), isSelected)) {
                tempSelectedLayer = i;
            }
        }
    }
    if (ImGui::CollapsingHeader("Discs")) {
        for (int i = 0; i < templateDiscs.size(); ++i) {
            bool isSelected = (tempSelectedDisc == i);
            if (ImGui::Selectable((templateDiscs[i].name + "##disc" + std::to_string(i)).c_str(), isSelected)) {
                tempSelectedDisc = i;
            }
        }
    }
    if (ImGui::CollapsingHeader("Drivers")) {
        for (int i = 0; i < (int)templateDrivers.size(); ++i) {
            bool isSelected = (tempSelectedDriver == i);
            if (ImGui::Selectable((templateDrivers[i].name + "##driver" + std::to_string(i)).c_str(), isSelected)) {
                tempSelectedDriver = i;
            }
        }
    }
    ImGui::EndChild();

    ImGui::NextColumn();
    ImGui::BeginChild("InfoColumn", ImVec2(0, 0), true);
    ImGui::Text("Part Details");

    // **Display Layer, Disc, Driver Details**
    if (tempSelectedLayer >= 0 && tempSelectedLayer < static_cast<int>(templateLayers.size())) {
        const auto& layer = templateLayers[tempSelectedLayer].part;
        ImGui::Separator();
        ImGui::Text("Name: %s", templateLayers[tempSelectedLayer].name.c_str());
        ImGui::Text("Model Path: %s", templateLayers[tempSelectedLayer].modelPath.c_str());
        ImGui::Text("Mass: %.3f kg", layer.mass);
    }
    if (tempSelectedDisc >= 0 && tempSelectedDisc < static_cast<int>(templateDiscs.size())) {
        const auto& disc = templateDiscs[tempSelectedDisc].part;
        ImGui::Separator();
        ImGui::Text("Name: %s", templateDiscs[tempSelectedDisc].name.c_str());
        ImGui::Text("Model Path: %s", templateDiscs[tempSelectedDisc].modelPath.c_str());
        ImGui::Text("Mass: %.3f kg", disc.mass);
    }
    if (tempSelectedDriver >= 0 && tempSelectedDriver < static_cast<int>(templateDrivers.size())) {
        const auto& driver = templateDrivers[tempSelectedDriver].part;
        ImGui::Separator();
        ImGui::Text("Name: %s", templateDrivers[tempSelectedDriver].name.c_str());
        ImGui::Text("Model Path: %s", templateDrivers[tempSelectedDriver].modelPath.c_str());
        ImGui::Text("Mass: %.3f kg", driver.mass);
    }

    ImGui::EndChild();

    // **Update and Reset Buttons**
    ImGui::Columns(1); // Reset to single column
    if (ImGui::Button("Update")) {
        // Validate selections
        if (tempSelectedLayer >= 0 && tempSelectedLayer < static_cast<int>(templateLayers.size()) &&
            tempSelectedDisc >= 0 && tempSelectedDisc < static_cast<int>(templateDiscs.size()) &&
            tempSelectedDriver >= 0 && tempSelectedDriver < static_cast<int>(templateDrivers.size())) {

            // Update Beyblade's template indices
            beyblade->templateIndices[0] = tempSelectedLayer;
            beyblade->templateIndices[1] = tempSelectedDisc;
            beyblade->templateIndices[2] = tempSelectedDriver;

            // TODO: Update Body and Mesh based on newly selected parts
            // beyblade->update(tempSelectedLayer, tempSelectedDisc, tempSelectedDriver)

            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Beyblade updated successfully!");
        }
        else {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid selection! Please choose valid parts.");
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset")) {
        tempSelectedLayer = beyblade->templateIndices[0];
        tempSelectedDisc = beyblade->templateIndices[1];
        tempSelectedDriver = beyblade->templateIndices[2];
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Selections and properties have been reset.");
    }
}

// Draw Popups
void CustomizeState::drawPopups() {
    // New Profile Popup
    if (currentPopup == PopupState::NEW_PROFILE && ImGui::BeginPopupModal("New Profile", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("##ProfileName", newProfileName, IM_ARRAYSIZE(newProfileName));

        if (ImGui::Button("Submit")) {
            game->pm.createProfile(newProfileName);
            game->pm.setActiveProfile(game->pm.getAllProfiles().back()->getId());
            newProfileName[0] = '\0';
            currentPopup = PopupState::NONE;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            newProfileName[0] = '\0';
            currentPopup = PopupState::NONE;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // New Beyblade Popup
    if (currentPopup == PopupState::NEW_BEYBLADE && ImGui::BeginPopupModal("New Beyblade", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Choose your type");
        if (ImGui::RadioButton("Manual", !isTemplate)) {
            isTemplate = false;
        }
        if (ImGui::RadioButton("Template", isTemplate)) {
            isTemplate = true;
        }

        ImGui::Text("Enter a name for the new Beyblade:");
        ImGui::InputText("##BeybladeName", newBeybladeName, IM_ARRAYSIZE(newBeybladeName));
        if (ImGui::Button("Submit")) {
            std::string beybladeNameStr(newBeybladeName);

            if (auto activeProfile = game->pm.getActiveProfile()) {
                if (isTemplate) {
                    activeProfile->createBeyblade(beybladeNameStr, true);
                }
                else {
                    activeProfile->createBeyblade(beybladeNameStr);
                }
                activeProfile->setActiveBeyblade(activeProfile->getAllBeyblades().back()->getId());
            }
            newBeybladeName[0] = '\0';
            currentPopup = PopupState::NONE;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            newBeybladeName[0] = '\0';
            currentPopup = PopupState::NONE;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Confirm Profile Deletion Popup
    if (currentPopup == PopupState::DELETE_PROFILE && ImGui::BeginPopupModal("Confirm Profile Deletion", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete the profile: %s", currentProfileName.c_str());
        if (ImGui::Button("Yes")) {
            if (!game->pm.deleteProfile(game->pm.getActiveProfile()->getId())) {
                std::cerr << "Cannot remove current profile (you must have at least one profile)" << std::endl;
            }
            currentPopup = PopupState::NONE;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No")) {
            currentPopup = PopupState::NONE;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Confirm Beyblade Deletion Popup
    if (currentPopup == PopupState::DELETE_BEYBLADE && ImGui::BeginPopupModal("Confirm Beyblade Deletion", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (auto activeProfile = game->pm.getActiveProfile()) {
            ImGui::Text("Are you sure you want to delete the Beyblade: %s", currentBeybladeName.c_str());
            if (ImGui::Button("Yes")) {
                if (!activeProfile->deleteBeyblade(activeProfile->getActiveBeyblade()->getId())) {
                    std::cerr << "Cannot delete current beyblade" << std::endl;
                }
                currentPopup = PopupState::NONE;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("No")) {
                currentPopup = PopupState::NONE;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
}