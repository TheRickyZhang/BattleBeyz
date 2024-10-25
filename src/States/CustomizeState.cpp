#include "CustomizeState.h"
#include "../GameEngine.h"
#include "StateIdentifiers.h"
#include "ProfileManager.h"

void CustomizeState::init() {}

void CustomizeState::cleanup() {}

void CustomizeState::pause() {}

void CustomizeState::resume() {}

void CustomizeState::handleEvents() {}

void CustomizeState::update(float deltaTime) {}

void CustomizeState::draw() {
    // Precompute values for layout
    float windowWidth = game->windowWidth;
    float buttonWidth = std::max(ImGui::CalcTextSize("Create New Profile").x, ImGui::CalcTextSize("Create New Beyblade").x)
        + ImGui::GetStyle().FramePadding.x * 2;
    float rightTextWidth = std::max(ImGui::CalcTextSize("Select Profile").x, ImGui::CalcTextSize("Select Beyblade").x)
        + ImGui::GetStyle().FramePadding.x * 2;
    float spacing = 20.0f;
    float dropdownRightEdge = windowWidth - rightTextWidth - spacing;
    float dropdownLeftX = buttonWidth + spacing;
    float dropdownWidth = dropdownRightEdge - dropdownLeftX;

    std::string currentProfileName = game->pm.getActiveProfile() ? game->pm.getActiveProfile()->getName() : "No Profile Selected";
    auto& beyblades = game->pm.getBeybladesForActiveProfile();


    // Cover full screen
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    // Line 1
    ImGui::Begin("Customize Menu");
    if (ImGui::Button("Back to Home")) {
        game->changeState(GameStateType::HOME);
    }

    // Line 2: [Create New Profile] <{Profile Name}...> Select Profile
    if (ImGui::Button("Create New Profile")) {
        showNewProfilePopup = true;
    }
    ImGui::SameLine();
    ImGui::SetCursorPosX(dropdownLeftX);
    ImGui::SetNextItemWidth(dropdownWidth);
    if (ImGui::BeginCombo("Select Profile", currentProfileName.c_str())) {
        auto& profiles = game->pm.getAllProfiles();
        for (auto& profile : profiles) {
            bool isSelected = (game->pm.getActiveProfile() == profile);
            if (ImGui::Selectable(profile->getName().c_str(), isSelected)) {
                game->pm.setActiveProfile(profile->getName());
                currentProfileName = profile->getName();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Line 3: [Create New Beyblade] <{Beyblade Name}...> Select Beyblade
    if (game->pm.getActiveProfile()) {
        std::string currentBeybladeName = beyblades.size() > 0 ? beyblades[currentIndex]->getName() : "No Beyblade Selected";
        if (ImGui::Button("Create New Beyblade")) {
            showNewBeybladePopup = true;
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(dropdownLeftX);
        ImGui::SetNextItemWidth(dropdownWidth);
        if (ImGui::BeginCombo("Select Beyblade", currentBeybladeName.c_str())) {
            for (int i = 0; i < (int)beyblades.size(); ++i) {
                bool isSelected = (currentIndex == i);
                if (ImGui::Selectable(beyblades[i]->getName().c_str(), isSelected)) {
                    currentIndex = i;
                    currentBeybladeName = beyblades[currentIndex]->getName();
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

    }
       
    // Line 4: Customize Your Beyblade || No Beyblades To Customize || Current Beyblade Not Selected
    if (beyblades.empty()) {
        ImGui::Text("No Beyblades to Customize");
        ImGui::End();
        return;
    }
    if (currentIndex < 0 || currentIndex >= (int)beyblades.size()) {
        ImGui::Text("Current Beyblade not selected");
        ImGui::End();
        return;
    }
    ImGui::Text("Customize Your Beyblade");

    auto& beyblade = beyblades[currentIndex];
    BeybladeBody* beybladeBody = beyblade->getRigidBody();


    // Lines 5-(n-1): Sliders for adjusting variables
    ImGui::SliderFloat("Layer Mass (kg)", &tempLayerMass, 0.010f, 0.099f, "%.3f");
    ImGui::SliderFloat("Layer Moment of Inertia (kg/m^2)", &tempLayerMOI, 0.000003f, 0.000016f, "%.9f");
    ImGui::SliderFloat("Driver Coefficient of Friction", &tempDriverCOF, 0.1f, 0.6f, "%.3f");

    // Line n: [Update]      [Reset]
    if (ImGui::Button("Update")) {
        beybladeBody->setLayerMass(tempLayerMass);
        beybladeBody->setLayerMomentOfInertia(tempLayerMOI);
        beybladeBody->setDriverCOF(tempDriverCOF);
    }
    ImGui::SameLine();
    ImGui::SetCursorPosX(windowWidth - rightTextWidth - spacing);
    if (ImGui::Button("Reset")) {
        tempLayerMass = (float)beybladeBody->getLayerMass();
        tempLayerMOI = (float)beybladeBody->getLayerMomentOfInertia();
        tempDriverCOF = (float)beybladeBody->getDriverCOF();
    }

    // Show popups if activated
    if (showNewProfilePopup) {
        ImGui::OpenPopup("New Profile");
    }
    if (showNewBeybladePopup) {
        ImGui::OpenPopup("New Beyblade");
    }
    if (ImGui::BeginPopupModal("New Profile", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("##ProfileName", newProfileName, IM_ARRAYSIZE(newProfileName));
        if (ImGui::Button("Submit")) {
            game->pm.addProfile(newProfileName);
            game->pm.setActiveProfile(newProfileName);
            showNewProfilePopup = false;
            newProfileName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            showNewProfilePopup = false;
            newProfileName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("New Beyblade", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter a name for the new Beyblade:");
        ImGui::InputText("##BeybladeName", newBeybladeName, IM_ARRAYSIZE(newBeybladeName));
        std::string beybladeNameStr(newBeybladeName);
        if (ImGui::Button("Submit")) {
            // Add the new beyblade to the active profile, set as currently selected, clear input/popup
            game->pm.getActiveProfile()->addBeyblade(beybladeNameStr);
            currentIndex = (int)beyblades.size() - 1;
            showNewBeybladePopup = false;
            newBeybladeName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            showNewBeybladePopup = false;
            newBeybladeName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::End();
}
