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
    // Note that padding for FRAME and ITEM need to be considered
    float windowWidth = game->windowWidth;
    float frameSpacing = ImGui::GetStyle().FramePadding.x;
    float spacing = ImGui::GetStyle().ItemSpacing.x;
    float leftTextWidth = std::max(ImGui::CalcTextSize("Profile").x, ImGui::CalcTextSize("Beyblade").x) + frameSpacing * 2;
    float rightButton2Width = ImGui::CalcTextSize("Delete").x + frameSpacing * 2;
    float rightButton1Width = ImGui::CalcTextSize("Create New").x + frameSpacing * 2;
    float rightButton1X = windowWidth - rightButton1Width - rightButton2Width - 2*spacing;
    float rightButton2X = windowWidth - rightButton2Width - spacing;
    float dropdownLeftX = leftTextWidth + spacing;
    float dropdownRightEdge = rightButton1X - spacing;
    float dropdownWidth = dropdownRightEdge - dropdownLeftX;

    std::string currentProfileName = game->pm.getActiveProfile() ? game->pm.getActiveProfile()->getName() : "No Profile Selected";
    auto& beyblades = game->pm.getBeybladesForActiveProfile();

    std::string beybladeName = currentIndex == -1 ? "" : beyblades[currentIndex]->getName();
    BeybladeBody* beybladeBody = currentIndex == -1 ? nullptr : beyblades[currentIndex]->getRigidBody();

    std::cout << "index" << currentIndex << std::endl;
    std::cout << int(currentPopup) << std::endl;
    // Cover full screen
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    // Line 1
    ImGui::Begin("Customize Menu");
    if (ImGui::Button("Back to Home")) {
        game->changeState(GameStateType::HOME);
    }

    // Line 2: [Profile] <{Profile Dropdown}...> [Create New] [Delete]
    ImGui::Text("Profile");
    ImGui::SameLine();
    ImGui::SetCursorPosX(dropdownLeftX);
    ImGui::SetNextItemWidth(dropdownWidth);
    if (ImGui::BeginCombo("##ProfileCombo", currentProfileName.c_str())) {
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
    ImGui::SameLine();
    ImGui::SetCursorPosX(rightButton1X);
    if (ImGui::Button("Create New")) {
        currentPopup = PopupState::NEW_PROFILE;
        ImGui::OpenPopup("New Profile");
    }
    ImGui::SameLine();
    ImGui::SetCursorPosX(rightButton2X);
    if (ImGui::Button("Delete")) {
        currentPopup = PopupState::DELETE_PROFILE;
        ImGui::OpenPopup("Confirm Profile Deletion");
    }

    // Line 3: [Beyblade] <{Beyblade Dropdown}...> [Create New] [Delete]
    if (game->pm.getActiveProfile()) {
        std::string currentBeybladeName = beyblades.size() > 0 ? beyblades[currentIndex]->getName() : "No Beyblade Selected";
        ImGui::Text("Beyblade");
        ImGui::SameLine();
        ImGui::SetCursorPosX(dropdownLeftX);
        ImGui::SetNextItemWidth(dropdownWidth);
        if (ImGui::BeginCombo("##BeybladeCombo", currentBeybladeName.c_str())) {
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
        ImGui::SameLine();
        ImGui::SetCursorPosX(rightButton1X);
        if (ImGui::Button("Create New")) {
            currentPopup = PopupState::NEW_BEYBLADE;
            ImGui::OpenPopup("New Beyblade");
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(rightButton2X);
        if (ImGui::Button("Delete")) {
            currentPopup = PopupState::DELETE_BEYBLADE;
            ImGui::OpenPopup("Confirm Beyblade Deletion");
            // TOFIX: Why does this not even print on click? And I suspect the beyblade row is not working
            std::cout << "clicked" << (int)currentPopup << std::endl;
        }
    }
       
    // Line 4: Customize Your Beyblade || No Beyblades To Customize || Current Beyblade Not Selected
    if (beyblades.empty()) {
        ImGui::Text("No Beyblades to Customize");
        ImGui::End();
        return;
    }
    if (beybladeName.empty()) {
        ImGui::Text("Current Beyblade not selected");
        ImGui::End();
        return;
    }
    ImGui::Text("Customize Your Beyblade");

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
    ImGui::SetCursorPosX(ImGui::CalcTextSize("Update").x + 2*spacing + 2*frameSpacing);
    if (ImGui::Button("Reset")) {
        tempLayerMass = (float)beybladeBody->getLayerMass();
        tempLayerMOI = (float)beybladeBody->getLayerMomentOfInertia();
        tempDriverCOF = (float)beybladeBody->getDriverCOF();
    }

    // Open popups based on currentPopup
    if (currentPopup == PopupState::NEW_PROFILE && ImGui::BeginPopupModal("New Profile", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("##ProfileName", newProfileName, IM_ARRAYSIZE(newProfileName));
        if (ImGui::Button("Submit")) {
            game->pm.addProfile(newProfileName);
            game->pm.setActiveProfile(newProfileName);
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

    if (currentPopup == PopupState::NEW_BEYBLADE && ImGui::BeginPopupModal("New Beyblade", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter a name for the new Beyblade:");
        ImGui::InputText("##BeybladeName", newBeybladeName, IM_ARRAYSIZE(newBeybladeName));
        std::string beybladeNameStr(newBeybladeName);
        if (ImGui::Button("Submit")) {
            // Add the new beyblade to the active profile, set as currently selected, clear input/popup
            game->pm.getActiveProfile()->addBeyblade(beybladeNameStr);
            currentIndex = (int)beyblades.size() - 1;
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

    // Confirmation Popup
    if (currentPopup == PopupState::DELETE_PROFILE && ImGui::BeginPopupModal("Confirm Profile Deletion", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete the profile: %s", currentProfileName.c_str());
        if (ImGui::Button("Yes")) {
            if (!game->pm.removeProfile(currentProfileName)) {
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

    if (currentPopup == PopupState::DELETE_BEYBLADE && currentIndex != -1 && ImGui::BeginPopupModal("Confirm Beyblade Deletion", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete the Beyblade: %s", beybladeName.c_str());
        if (ImGui::Button("Yes")) {
            if (!game->pm.getActiveProfile()->deleteBeyblade(beybladeName)) {
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
        ImGui::EndPopup();
    }


    ImGui::End();
}
