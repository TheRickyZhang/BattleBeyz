#include "CustomizeState.h"

#include "StateIdentifiers.h"
#include "BeybladeConstants.h"
#include "Beyblade.h"
#include "Stadium.h"
#include "ProfileManager.h"
#include "ImGuiUI.h"
#include "../lib/ImGuiFileDialog/ImGuiFileDialog.h"
#include "../lib/ImGuiFileDialog/ImGuiFileDialogConfig.h"
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace ImGui;

void CustomizeState::init() {
    leftTextWidth = max({ CalcTextSize("Profile").x, CalcTextSize("Beyblade").x, CalcTextSize("Stadium").x }) + frameSpacingX * 2;
    rightButton2Width = CalcTextSize("Delete##profile").x + frameSpacingX * 2;
    rightButton1Width = CalcTextSize("Create New").x + frameSpacingX * 2;

    if (game->pm.getActiveProfile()) {
        auto activeStadium = game->pm.getActiveProfile()->getActiveStadium();
        stadiumPreview = std::make_unique<StadiumPreview>(
            380, 380, // TOLOOK: This should be - 2 buffer
            game->physicsWorld,
            game->objectShader
        );
    }

    onResize(game->windowWidth, game->windowHeight);
}

void CustomizeState::cleanup() {}

void CustomizeState::pause() {}

void CustomizeState::resume() {}

void CustomizeState::handleEvents() {
    if (stadiumPreview) {
        stadiumPreview->handleInput(game->deltaTime);
    }
}

// TODO: use onResize() in other states
void CustomizeState::onResize(int width, int height) {
    rightButton1X = width - rightButton1Width - rightButton2Width - 2 * spacing;
    rightButton2X = width - rightButton2Width - spacing;
    dropdownLeftX = leftTextWidth + spacing;
    dropdownWidth = rightButton1X - spacing - dropdownLeftX;
}

void CustomizeState::update(float deltaTime) {
    stadiumPreview->update(deltaTime, game->currTime);
}

// Draw Method
void CustomizeState::draw() {
    // Initialize data for rendering
    vector<shared_ptr<Profile>>  profiles;    shared_ptr<Profile>  profile = nullptr;
    vector<shared_ptr<Beyblade>> beyblades;   shared_ptr<Beyblade> beyblade = nullptr;
    vector<shared_ptr<Stadium>>  stadiums;    shared_ptr<Stadium>  stadium = nullptr;
    initializeData(profiles, profile, beyblades, beyblade, stadiums, stadium);

    // Cover full screen
    auto [centerX, wrapWidth] = SetWindowPositionAndSize(1, 1, 1, 1);
    Begin("Customize Menu", nullptr, ScrollableWindow);

    // Draw "Back to Home" button
    if (Button("Back to Home")) {
        game->changeState(StateFactory::createState(game, GameStateType::HOME));
        End();  // Ensures proper window closure
        return;
    }

    // Draw Profile Section
    profile = drawProfileSection(profiles, profile);

    // Draw Beyblade Section if a profile is active
    if (profile) {
        beyblade = drawBeybladeSection(beyblades, beyblade, profile);
        stadium = drawStadiumSection(stadiums, stadium, profile);
    }
    SeparatorSpacedThick();

    // Draw Customize Section
    if (!profile) {
        Text("No Profiles Found");
    }
    else {
        if (!beyblade) {
            Text("No Beyblades Found");
        }
        else if (beyblade->isTemplate) {
            drawTemplateCustomizeSection(beyblade);
        }
        else {
            drawManualCustomizeSection(beyblade);
        }
        SeparatorSpacedThick();
        if (!stadium) {
            Text("No Stadiums Found");
        }
        else {
            drawStadiumCustomizeSection(stadium);
        }
    }

    // Handle Popups
    drawPopups(profile, beyblade, stadium);

    // End main window
    End();
}

// Initialize data for rendering
// PERF: This data only needs to be recomputed on initialization or change in dropdown selection. Low priority.
void CustomizeState::initializeData(vector<shared_ptr<Profile>>& profiles, shared_ptr<Profile>& profile,
    vector<shared_ptr<Beyblade>>& beyblades, shared_ptr<Beyblade>& beyblade,
    vector<shared_ptr<Stadium>>& stadiums, shared_ptr<Stadium>& stadium) {
    profiles = game->pm.getAllProfiles();
    profile = game->pm.getActiveProfile();
    if (profile) {
        beyblades = profile->getAllBeyblades();
        stadiums = profile->getAllStadiums();
        currentProfileName = profile->getName();
        beyblade = beyblades.empty() ? nullptr : profile->getActiveBeyblade();
        stadium = stadiums.empty() ? nullptr : profile->getActiveStadium();
        currentBeybladeName = beyblade ? beyblade->getName() : "No Beyblade Selected";
        currentStadiumName = stadium ? stadium->getName() : "No Stadium Selected";
    }
    else {
        currentProfileName = "No Profile Selected"; currentBeybladeName = "No Beyblade Selected"; currentStadiumName = "No Stadium Selected";
    }
}

// Draw Profile Section
shared_ptr<Profile> CustomizeState::drawProfileSection(const vector<shared_ptr<Profile>>& profiles,
                                                            const shared_ptr<Profile>& activeProfile) {
    shared_ptr<Profile> newProfile = drawSection<Profile>(
        "Profile", "##ProfileCombo", profiles, activeProfile,
        [&](const shared_ptr<Profile>& profile) { game->pm.setActiveProfile(profile->getId()); },
        [](const shared_ptr<Profile>& profile) { return profile->getName(); },
        [&]() {
            currentPopup = PopupState::NEW_PROFILE;
            OpenPopup("New Profile");
        },
        [&]() {
            currentPopup = PopupState::DELETE_PROFILE;
            OpenPopup("Confirm Profile Deletion");
        }
    );
    if (newProfile != activeProfile) {
        stadiumPreview->stadium = (newProfile ? newProfile->getActiveStadium().get() : nullptr);
    }
    return newProfile;
}


// Draw Beyblade Section
shared_ptr<Beyblade> CustomizeState::drawBeybladeSection(
    const vector<shared_ptr<Beyblade>>& beyblades,
    const shared_ptr<Beyblade>& activeBeyblade,
    const shared_ptr<Profile>& profile) {

    return drawSection<Beyblade>(
        "Beyblade", "##BeybladeCombo", beyblades, activeBeyblade,
        [&](const shared_ptr<Beyblade>& beyblade) {
            profile->setActiveBeyblade(beyblade->getId());
            currentBeybladeName = beyblade->getName();
        },
        [](const shared_ptr<Beyblade>& beyblade) { return beyblade->getName(); },
        [&]() {
            currentPopup = PopupState::NEW_BEYBLADE;
            OpenPopup("New Beyblade");
        },
        [&]() {
            currentPopup = PopupState::DELETE_BEYBLADE;
            OpenPopup("Confirm Beyblade Deletion");
        }
    );
}

shared_ptr<Stadium> CustomizeState::drawStadiumSection(
    const vector<shared_ptr<Stadium>>& stadiums,
    const shared_ptr<Stadium>& activeStadium,
    const shared_ptr<Profile>& profile) {

    shared_ptr<Stadium> newStadium = drawSection<Stadium>(
        "Stadium", "##StadiumCombo", stadiums, activeStadium,
        [&](const shared_ptr<Stadium>& stadium) {
            profile->setActiveStadium(stadium->getId());
            currentStadiumName = stadium->getName();
        },
        [](const shared_ptr<Stadium>& stadium) { return stadium->getName(); },
        [&]() {
            currentPopup = PopupState::NEW_STADIUM;
            OpenPopup("New Stadium");
        },
        [&]() {
            currentPopup = PopupState::DELETE_STADIUM;
            OpenPopup("Confirm Stadium Deletion");
        }
    );

    // Update the stadium preview if the active stadium changes
    if (newStadium != activeStadium) {
        stadiumPreview->stadium = newStadium.get();
    }

    return newStadium;
}


// Line 4: Customize Your Beyblade || No Beyblades To Customize || Current Beyblade Not Selected
void CustomizeState::drawManualCustomizeSection(shared_ptr<Beyblade> beyblade) {
    // We assume from prior checks that beybladeBody is never nullptr here
    BeybladeBody* beybladeBody = beyblade->getBody();

    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Customize Your Beyblade: ");

    ImGui::SameLine();

    SetCursorPosX(GetCursorPosX() + 50);
    // Show update button in red if there are active changes
    const ImVec4& buttonModified = ImVec4(0.6f, 0.0f, 0.0f, 1.0f);
    bool modified = beybladeBody->getModified();
    if (modified) PushStyleColor(ImGuiCol_Button, buttonModified);
    if (Button("Save Beyblade")) {
        ScalarParameter::assignToBeybladeBody(beybladeBody);
        beybladeBody->setModified(false);
    }
    if (modified) PopStyleColor();

    ImGui::SameLine();

    float updateButtonWidth = CalcTextSize("Update").x + 2 * GetStyle().ItemSpacing.x + 2 * GetStyle().FramePadding.x;
    SetCursorPosX(GetCursorPosX() + 50);
    if (Button("Reset Beyblade")) {
        ScalarParameter::assignFromBeybladeBody(beybladeBody);
        beybladeBody->setModified(false);
    }

    SeparatorSpaced();

    // Update temporary variables if beyblade has changed
    if (beybladeBody != prevbladeBody) {
        ScalarParameter::assignFromBeybladeBody(beybladeBody);
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
            string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            auto newMesh = make_unique<BeybladeMesh>(filePathName.c_str());  // This loads the mesh.
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

    for (ScalarParameter& layer : layerParameters) {
        DrawDiscreteFloatControl(layer.name.c_str(), getMaxLayerTextSize(), "layer", layer.currentValue, layer.minValue, layer.maxValue,
            layer.getStepSize(), layer.getFastStepSize(), layer.getDisplayFormat().c_str(), onModified);
    }
    SeparatorSpaced();
    for (ScalarParameter& disc : discParameters) {
        DrawDiscreteFloatControl(disc.name.c_str(), getMaxDiscTextSize(), "disc", disc.currentValue, disc.minValue, disc.maxValue,
            disc.getStepSize(), disc.getFastStepSize(), disc.getDisplayFormat().c_str(), onModified);
    }
    SeparatorSpaced();
    for (ScalarParameter& driver : driverParameters) {
        DrawDiscreteFloatControl(driver.name.c_str(), getMaxDriverTextSize(), "driver", driver.currentValue, driver.minValue, driver.maxValue,
            driver.getStepSize(), driver.getFastStepSize(), driver.getDisplayFormat().c_str(), onModified);
    }

    // Line n: Update and Reset Buttons
    SeparatorSpacedThick();
}

void CustomizeState::drawTemplateCustomizeSection(shared_ptr<Beyblade> beyblade) {
    Text("Choose your parts (Layer, Disc, Driver)");
    /*
    *  Layer  |  Info
    *  Disc   |  Info
    *  Driver |  Info
    */
    Columns(2, nullptr, true);

    ImGui::BeginChild("SelectionColumn", ImVec2(0, 0), true);
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
    ImGui::EndChild();

    NextColumn();
    ImGui::BeginChild("InfoColumn", ImVec2(0, 0), true);
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
        Text("Mass: %.3f kg", layer->mass);
    }
    if (isDiscSelected) {
        const auto& disc = templateDiscs[tempSelectedDisc].part;
        Separator();
        Text("Name: %s", templateDiscs[tempSelectedDisc].name.c_str());
        Text("Model Path: %s", templateDiscs[tempSelectedDisc].modelPath.c_str());
        Text("Mass: %.3f kg", disc->mass);
    }
    if (isDriverSelected) {
        const auto& driver = templateDrivers[tempSelectedDriver].part;
        Separator();
        Text("Name: %s", templateDrivers[tempSelectedDriver].name.c_str());
        Text("Model Path: %s", templateDrivers[tempSelectedDriver].modelPath.c_str());
        Text("Mass: %.3f kg", driver->mass);
    }

    ImGui::EndChild();

    // **Update and Reset Buttons**
    Columns(1); // Reset to single column
    if (Button("Update Beyblade (Template)")) {
        // Validate selections
        if (isLayerSelected && isDiscSelected && isDriverSelected) {
            beyblade->update(tempSelectedLayer, tempSelectedDisc, tempSelectedDriver);
            TextColored(ImVec4(0, 1, 0, 1), "Beyblade updated successfully!");
        }
        else {
            TextColored(ImVec4(1, 0, 0, 1), "Invalid selection! Please choose valid parts.");
        }
    }

    ImGui::SameLine();

    if (Button("Reset Beyblade (Template)")) {
        tempSelectedLayer = beyblade->templateIndices[0];
        tempSelectedDisc = beyblade->templateIndices[1];
        tempSelectedDriver = beyblade->templateIndices[2];
        TextColored(ImVec4(0, 1, 0, 1), "Selections and properties have been reset.");
    }
}

void CustomizeState::drawStadiumCustomizeSection(shared_ptr<Stadium> stadium) {

    // Ensure stadium is valid
    if (!stadium) {
        ImGui::Text("No stadium selected!");
        return;
    }
    Stadium* stadiumBody = stadium.get();


    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Customize Your Stadium:");
    ImGui::SameLine();


    // Show update button in red if there are active changes
    const ImVec4& buttonModified = ImVec4(0.6f, 0.0f, 0.0f, 1.0f);
    bool modified = stadium->getModified();
    if (modified) ImGui::PushStyleColor(ImGuiCol_Button, buttonModified);
    if (ImGui::Button("Save Stadium")) {
        ScalarParameter::assignToStadium(stadiumBody);
        Vec3Parameter::assignToStadium(stadiumBody);
        ScalarParameter::assignToStadium(stadiumPreview->stadium);
        Vec3Parameter::assignToStadium(stadiumPreview->stadium);
        stadium->setModified(false);
    }
    if (modified) ImGui::PopStyleColor();

    ImGui::SameLine();

    // Always sync stadiumPreview and the extern data in stadiumPreview. Annoying and prehaps not scalable, but easiest solution that works
    if (ImGui::Button("Reset Stadium")) {

        cout << "Clicked" << endl;
        ScalarParameter::assignFromStadium(stadiumBody);           Vec3Parameter::assignFromStadium(stadiumBody);
        cout << "ScalarParameter for radius is now" << stadiumParameters[0].currentValue << endl;
        ScalarParameter::assignToStadium(stadiumPreview->stadium); Vec3Parameter::assignToStadium(stadiumPreview->stadium);
        stadium->setModified(false);
    }

    SeparatorSpaced();

    // Update temporary variables if stadium changes
    if (stadiumBody != prevStadium) {
        cout << "HUHUHUHU" << endl;
        ScalarParameter::assignFromStadium(stadiumBody);           Vec3Parameter::assignFromStadium(stadiumBody);
        ScalarParameter::assignToStadium(stadiumPreview->stadium); Vec3Parameter::assignToStadium(stadiumPreview->stadium);
        prevStadium = stadium.get();
    }

    auto onModified = [&]() { stadium->setModified(); };

    ImGui::BeginChild("StadiumPreview", ImVec2(400, 400), true, ImGuiWindowFlags_NoScrollbar);
    {
        if (stadiumPreview) {
            stadiumPreview->draw();
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("StadiumSettings", ImVec2(0, 400), true, ImGuiWindowFlags_NoScrollbar);
    {
        // Render scalar sliders
        for (ScalarParameter& param : stadiumParameters) {
            if (DrawDiscreteFloatControl(param.name.c_str(), getMaxStadiumTextSize(), "stadium", param.currentValue,
                param.minValue, param.maxValue, param.getStepSize(), param.getFastStepSize(),
                param.getDisplayFormat().c_str(), onModified)) {
                ScalarParameter::assignToStadium(stadiumPreview->stadium); Vec3Parameter::assignToStadium(stadiumPreview->stadium);
            }
        }
        SeparatorSpaced();

        // Render Vec3 color pickers
        for (Vec3Parameter& vecParam : stadiumVec3Parameters) {
            ImGui::Text(vecParam.name.c_str());
            ImGui::SameLine();
            if (ImGui::ColorEdit3(("##" + vecParam.name).c_str(), glm::value_ptr(vecParam.currentValue))) {
                onModified();
            }
        }
    }
    ImGui::EndChild();

    // Line n: Final separator
    SeparatorSpacedThick();
}



// NOTE: Since drawPopups is the last thing called in draw(), no need to update value of beyblade and profile
void CustomizeState::drawPopups(const shared_ptr<Profile>& profile, const shared_ptr<Beyblade>& beyblade, const shared_ptr<Stadium>& stadium) {
    ProfileManager& pm = game->pm;

    auto drawConfirmDeletionPopup = [&](const string& itemName, const string& popupTitle, function<bool()> deleteFunc) {
        if (BeginPopupModal(popupTitle.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            Text("Are you sure you want to delete: %s", itemName.c_str());
            if (Button("Yes")) {
                if (!deleteFunc()) {
                    game->ml.addMessage("Error Deleting " + itemName, MessageType::ERROR, true);
                }
                currentPopup = PopupState::NONE;
                CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (Button("No")) {
                currentPopup = PopupState::NONE;
                CloseCurrentPopup();
            }
            EndPopup();
        }
        };

    auto drawNewItemPopup = [&](const string& popupTitle, const char* inputLabel, char* inputBuffer, int bufferSize,
        function<bool(const string&)> createFunc) {
            if (BeginPopupModal(popupTitle.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                Text("Name: "); ImGui::SameLine();
                InputText(inputLabel, inputBuffer, bufferSize);
                if (Button("Submit")) {
                    if (strlen(inputBuffer) == 0) {
                        game->ml.addMessage(popupTitle + " name cannot be empty", MessageType::WARNING, true);
                    }
                    else {
                        string nameStr(inputBuffer);
                        if (!createFunc(nameStr)) {
                            game->ml.addMessage("Cannot create new " + popupTitle, MessageType::ERROR, true);
                        }
                        inputBuffer[0] = '\0';
                        currentPopup = PopupState::NONE;
                        CloseCurrentPopup();
                    }
                }
                ImGui::SameLine();
                if (Button("Cancel")) {
                    inputBuffer[0] = '\0';
                    currentPopup = PopupState::NONE;
                    CloseCurrentPopup();
                }
                EndPopup();
            }
        };


    // Popups for creating new item
    if (currentPopup == PopupState::NEW_PROFILE) {
        drawNewItemPopup("New Profile", "##ProfileName", newProfileName, IM_ARRAYSIZE(newProfileName),
            [&](const string& name) {
                if (!pm.createProfile(name)) return false;
                pm.setActiveProfile(pm.getAllProfiles().back()->getId());
                return true;
            });
    }
    if (currentPopup == PopupState::NEW_BEYBLADE) {
        if (BeginPopupModal("New Beyblade", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            Text("Choose your type");
            if (RadioButton("Manual", !isTemplate)) isTemplate = false;
            if (RadioButton("Template", isTemplate)) isTemplate = true;

            Text("Name: "); ImGui::SameLine();
            InputText("##BeybladeName", newBeybladeName, IM_ARRAYSIZE(newBeybladeName));
            if (Button("Submit")) {
                if (strlen(newBeybladeName) == 0) {
                    game->ml.addMessage("Beyblade name cannot be empty", MessageType::WARNING, true);
                }
                else if (profile) {
                    string name(newBeybladeName);
                    bool created = isTemplate ? profile->createBeyblade(name, true) : profile->createBeyblade(name);
                    if (!created) {
                        game->ml.addMessage("Cannot create new Beyblade", MessageType::ERROR, true);
                    }
                    else {
                        profile->setActiveBeyblade(profile->getAllBeyblades().back()->getId());
                    }
                    newBeybladeName[0] = '\0';
                    currentPopup = PopupState::NONE;
                    CloseCurrentPopup();
                }
            }
            ImGui::SameLine();
            if (Button("Cancel")) {
                newBeybladeName[0] = '\0';
                currentPopup = PopupState::NONE;
                CloseCurrentPopup();
            }
            EndPopup();
        }
    }
    if (currentPopup == PopupState::NEW_STADIUM) {
        drawNewItemPopup("New Stadium", "##StadiumName", newStadiumName, IM_ARRAYSIZE(newStadiumName),
            [&](const string& name) {
                if (!profile->createStadium(name)) return false;
                profile->setActiveStadium(profile->getAllStadiums().back()->getId());
                return true;
            });
    }

    // Popups for deleting
    if (currentPopup == PopupState::DELETE_PROFILE) {
        drawConfirmDeletionPopup(currentProfileName, "Confirm Profile Deletion", [&]() {return pm.deleteProfile(profile->getId());});
    }
    if (currentPopup == PopupState::DELETE_BEYBLADE) {
        drawConfirmDeletionPopup(currentBeybladeName, "Confirm Beyblade Deletion", [&]() {return profile->deleteBeyblade(beyblade->getId());});
    }
    if (currentPopup == PopupState::DELETE_STADIUM) {
        drawConfirmDeletionPopup(currentStadiumName, "Confirm Stadium Deletion", [&]() {return profile->deleteStadium(stadium->getId());});
    }
}