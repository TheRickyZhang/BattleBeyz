#include "CustomizeState.h"
#include "../GameEngine.h"
#include "StateIdentifiers.h"
#include "ProfileManager.h"

void CustomizeState::init() {}

void CustomizeState::cleanup() {}

void CustomizeState::pause() {}

void CustomizeState::resume() {}

void CustomizeState::handleEvents(GameEngine* game) {}

void CustomizeState::update(GameEngine* game, float deltaTime) {}

void CustomizeState::draw(GameEngine* game) {
    // Set the window to cover the entire screen
    ImGui::SetNextWindowPos(ImVec2(0, 0)); // Top-left corner
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize); // Full screen size

    // Add padding between the window edges and the content
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(50, 50)); // Adjust padding as needed

    ImGui::Begin("Customize Menu");

    if (ImGui::Button("Back to Home")) {
        game->changeState(GameStateType::HOME);
    }

    std::string currentProfileName = game->pm.getActiveProfile() ? game->pm.getActiveProfile()->getName() : "No Profile Selected";

    // Open popup for new profile if button pressed
    if (ImGui::Button("Create New Profile")) {
        showNewProfilePopup = true;
    }
    if (showNewProfilePopup) {
        ImGui::OpenPopup("New Profile");
    }

    // Calculate available width and set cursor position for aligning combo box to the right
    float availableWidth = ImGui::GetContentRegionAvail().x;
    float comboWidth = 250.0f;  // Adjust combo box width as needed
    ImGui::SameLine();
    ImGui::SetCursorPosX(availableWidth - comboWidth);

    if (ImGui::BeginCombo("Select Profile", currentProfileName.c_str())) {
        auto& profiles = game->pm.getAllProfiles();  // Assume this returns a vector of profiles
        for (auto& profile : profiles) {
            bool isSelected = (game->pm.getActiveProfile() == profile);
            if (ImGui::Selectable(profile->getName().c_str(), isSelected)) {
                game->pm.setActiveProfile(profile->getName());  // Set the active profile in ProfileManager
                currentProfileName = profile->getName();        // Update the displayed profile name
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }



    if (ImGui::BeginPopupModal("New Profile", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter a name for the new profile:");
        ImGui::InputText("##ProfileName", newProfileName, IM_ARRAYSIZE(newProfileName));

        // Submit button to create the new profile
        if (ImGui::Button("Submit")) {
            game->pm.addProfile(newProfileName);          // Add the new profile
            game->pm.setActiveProfile(newProfileName);    // Set the new profile as active
            showNewProfilePopup = false;                  // Close the popup
            newProfileName[0] = '\0';                     // Clear the input buffer
            ImGui::CloseCurrentPopup();                   // Close the modal
        }
        ImGui::SameLine();

        // Cancel button to close the popup
        if (ImGui::Button("Cancel")) {
            showNewProfilePopup = false;                  // Close the popup
            newProfileName[0] = '\0';                     // Clear the input buffer
            ImGui::CloseCurrentPopup();                   // Close the modal
        }

        ImGui::EndPopup();
    }

    // BEYBLADE DROPDOWN LOGIC
    if (game->pm.getActiveProfile()) { // Ensure an active profile exists
        auto& beyblades = game->pm.getBeybladesForActiveProfile(); // Retrieve beyblades from the active profile
        const char* currentBeybladeName = beyblades.size() > 0 ? beyblades[currentIndex]->getName().c_str() : "No Beyblade Selected";

        // Button to create a new beyblade
        if (ImGui::Button("Create New Beyblade")) {
            showNewBeybladePopup = true;  // Open the beyblade creation popup
        }

        // Calculate available width and set cursor position for aligning combo box to the right
        float availableWidth = ImGui::GetContentRegionAvail().x;
        float comboWidth = 200.0f;  // Adjust combo box width as needed
        ImGui::SameLine();
        ImGui::SetCursorPosX(availableWidth - comboWidth);

        if (ImGui::BeginCombo("Select Beyblade", currentBeybladeName)) {
            for (int i = 0; i < beyblades.size(); ++i) {
                bool isSelected = (currentIndex == i);
                if (ImGui::Selectable(beyblades[i]->getName().c_str(), isSelected)) {
                    currentIndex = i;  // Update the current beyblade selection
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }



        // Open the new Beyblade popup if button pressed
        if (showNewBeybladePopup) {
            ImGui::OpenPopup("New Beyblade");
        }

        if (ImGui::BeginPopupModal("New Beyblade", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Enter a name for the new Beyblade:");
            ImGui::InputText("##BeybladeName", newBeybladeName, IM_ARRAYSIZE(newBeybladeName));
            std::string beybladeNameStr(newBeybladeName);  // Convert char[] to std::string

            // Submit button to create the new Beyblade
            if (ImGui::Button("Submit")) {
                game->pm.getActiveProfile()->addBeyblade(beybladeNameStr); // Add the new beyblade to the active profile
                currentIndex = beyblades.size() - 1;  // Set the new Beyblade as the current selection
                showNewBeybladePopup = false;         // Close the popup
                newBeybladeName[0] = '\0';            // Clear the input buffer
                ImGui::CloseCurrentPopup();           // Close the modal
            }
            ImGui::SameLine();

            // Cancel button to close the popup
            if (ImGui::Button("Cancel")) {
                showNewBeybladePopup = false;  // Close the popup
                newBeybladeName[0] = '\0';     // Clear the input buffer
                ImGui::CloseCurrentPopup();    // Close the modal
            }

            ImGui::EndPopup();
        }
    }

    // Get the active profile's beyblades
    auto& beyblades = game->pm.getBeybladesForActiveProfile();
    if (currentIndex >= beyblades.size() || currentIndex < 0) {
        ImGui::Text("Current Beyblade not selected");
        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }
    auto& beyblade = beyblades[currentIndex];
    BeybladeBody* beybladeBody = beyblade->getRigidBody();

    ImGui::Text("Customize Your Beyblade");

    if (beyblades.empty()) {
        ImGui::Text("No Beyblades to Customize");
        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }

    // Sliders for adjusting variables (using the member variables)
    ImGui::SliderFloat("Layer Mass (kg)", &tempLayerMass, 0.010f, 0.099f);
    ImGui::SliderFloat("Layer Moment of Inertia (kg/m^2)", &tempLayerMOI, 0.000003f, 0.000016f);
    ImGui::SliderFloat("Driver Coefficient of Friction", &tempDriverCOF, 0.1f, 0.6f);

    // Add "Update" button
    if (ImGui::Button("Update")) {
        beybladeBody->setLayerMass(tempLayerMass);
        beybladeBody->setLayerMomentOfInertia(tempLayerMOI);
        beybladeBody->setDriverCOF(tempDriverCOF);
    }

    // Initialize the sliders with values from BeybladeBody
    if (ImGui::Button("Reset")) {
        tempLayerMass = beybladeBody->getLayerMass();
        tempLayerMOI = beybladeBody->getLayerMomentOfInertia();
        tempDriverCOF = beybladeBody->getDriverCOF();
    }

    ImGui::End();

    ImGui::PopStyleVar();
}
