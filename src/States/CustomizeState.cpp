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
    leftTextWidth = std::max({ CalcTextSize("Profile").x, CalcTextSize("Beyblade").x, CalcTextSize("Stadium").x }) + frameSpacingX * 2;
    rightButton2Width = CalcTextSize("Delete##profile").x + frameSpacingX * 2;
    rightButton1Width = CalcTextSize("Create New").x + frameSpacingX * 2;

    stadiumPreview = std::make_unique<StadiumPreview>(300, 300, game->physicsWorld, game->objectShader);

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
    SetWindowPositionAndSize(1, 1, 1, 1);

    // Begin main window
    Begin("Customize Menu");

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
std::shared_ptr<Profile> CustomizeState::drawProfileSection(
    const vector<std::shared_ptr<Profile>>& profiles,
    const std::shared_ptr<Profile>& activeProfile) {

    return drawSection<Profile>(
        "Profile", "##ProfileCombo", profiles, activeProfile,
        [&](const std::shared_ptr<Profile>& profile) { game->pm.setActiveProfile(profile->getId()); },
        [](const std::shared_ptr<Profile>& profile) { return profile->getName(); },
        [&]() {
            currentPopup = PopupState::NEW_PROFILE;
            OpenPopup("New Profile");
        },
        [&]() {
            currentPopup = PopupState::DELETE_PROFILE;
            OpenPopup("Confirm Profile Deletion");
        }
    );
}


// Draw Beyblade Section
std::shared_ptr<Beyblade> CustomizeState::drawBeybladeSection(
    const vector<std::shared_ptr<Beyblade>>& beyblades,
    const std::shared_ptr<Beyblade>& activeBeyblade,
    const std::shared_ptr<Profile>& profile) {

    return drawSection<Beyblade>(
        "Beyblade", "##BeybladeCombo", beyblades, activeBeyblade,
        [&](const std::shared_ptr<Beyblade>& beyblade) {
            profile->setActiveBeyblade(beyblade->getId());
            currentBeybladeName = beyblade->getName();
        },
        [](const std::shared_ptr<Beyblade>& beyblade) { return beyblade->getName(); },
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

std::shared_ptr<Stadium> CustomizeState::drawStadiumSection(
    const vector<std::shared_ptr<Stadium>>& stadiums,
    const std::shared_ptr<Stadium>& activeStadium,
    const std::shared_ptr<Profile>& profile) {

    return drawSection<Stadium>(
        "Stadium", "##StadiumCombo", stadiums, activeStadium,
        [&](const std::shared_ptr<Stadium>& stadium) {
            profile->setActiveStadium(stadium->getId());
            currentStadiumName = stadium->getName();
        },
        [](const std::shared_ptr<Stadium>& stadium) { return stadium->getName(); },
        [&]() {
            currentPopup = PopupState::NEW_STADIUM;
            OpenPopup("New Stadium");
        },
        [&]() {
            currentPopup = PopupState::DELETE_STADIUM;
            OpenPopup("Confirm Stadium Deletion");
        }
    );
}



// Line 4: Customize Your Beyblade || No Beyblades To Customize || Current Beyblade Not Selected
void CustomizeState::drawManualCustomizeSection(shared_ptr<Beyblade> beyblade) {
    // We assume from prior checks that beybladeBody is never nullptr here
    BeybladeBody* beybladeBody = beyblade->getBody();

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

void CustomizeState::drawStadiumCustomizeSection(std::shared_ptr<Stadium> stadium) {
    Text("Stadium and Physics");
    Separator();

    if (!stadium) {
        Text("No stadium selected!");
        return;
    }

    Columns(2, nullptr, false);
    BeginChild("StadiumPreview", ImVec2(0, 0), true);
    {
        Text("Preview:");
        if (stadiumPreview) {
            stadiumPreview->draw();
        }
    }
    EndChild();

    NextColumn();

    BeginChild("StadiumSettings", ImVec2(0, 0), true);
    {
        Text("Settings:");
        // Access the stadium from the preview
        shared_ptr<Stadium> st = stadiumPreview->getStadium();
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
            *stadium.get() = *st.get();
        }
        SameLine();
        if (Button("Undo Changes")) {
            *st.get() = *stadium.get();
        }
    }
    EndChild();

    Columns(1);
}


// NOTE: Since drawPopups is the last thing called in draw(), no need to update value of beyblade and profile
void CustomizeState::drawPopups(const shared_ptr<Profile>& profile, const shared_ptr<Beyblade>& beyblade, const shared_ptr<Stadium>& stadium) {
    ProfileManager& pm = game->pm;

    auto drawConfirmDeletionPopup = [&](const std::string& itemName, const std::string& popupTitle, std::function<bool()> deleteFunc) {
        if (BeginPopupModal(popupTitle.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            Text("Are you sure you want to delete: %s", itemName.c_str());
            if (Button("Yes")) {
                if (!deleteFunc()) {
                    game->ml.addMessage("Error Deleting " + itemName, MessageType::ERROR, true);
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
        };

    auto drawNewItemPopup = [&](const std::string& popupTitle, const char* inputLabel, char* inputBuffer, int bufferSize,
        std::function<bool(const std::string&)> createFunc) {
            if (BeginPopupModal(popupTitle.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                Text("Name: "); SameLine();
                InputText(inputLabel, inputBuffer, bufferSize);
                if (Button("Submit")) {
                    if (strlen(inputBuffer) == 0) {
                        game->ml.addMessage(popupTitle + " name cannot be empty", MessageType::WARNING, true);
                    }
                    else {
                        std::string nameStr(inputBuffer);
                        if (!createFunc(nameStr)) {
                            game->ml.addMessage("Cannot create new " + popupTitle, MessageType::ERROR, true);
                        }
                        inputBuffer[0] = '\0';
                        currentPopup = PopupState::NONE;
                        CloseCurrentPopup();
                    }
                }
                SameLine();
                if (Button("Cancel")) {
                    inputBuffer[0] = '\0';
                    currentPopup = PopupState::NONE;
                    CloseCurrentPopup();
                }
                EndPopup();
            }
        };

    // New Profile Popup
    if (currentPopup == PopupState::NEW_PROFILE) {
        drawNewItemPopup("New Profile", "##ProfileName", newProfileName, IM_ARRAYSIZE(newProfileName),
            [&](const std::string& name) {
                if (!pm.createProfile(name)) return false;
                pm.setActiveProfile(pm.getAllProfiles().back()->getId());
                return true;
            });
    }

    // New Beyblade Popup: Special case
    if (currentPopup == PopupState::NEW_BEYBLADE) {
        if (BeginPopupModal("New Beyblade", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            Text("Choose your type");
            if (RadioButton("Manual", !isTemplate)) isTemplate = false;
            if (RadioButton("Template", isTemplate)) isTemplate = true;

            Text("Name: "); SameLine();
            InputText("##BeybladeName", newBeybladeName, IM_ARRAYSIZE(newBeybladeName));
            if (Button("Submit")) {
                if (strlen(newBeybladeName) == 0) {
                    game->ml.addMessage("Beyblade name cannot be empty", MessageType::WARNING, true);
                }
                else if (profile) {
                    std::string name(newBeybladeName);
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
            SameLine();
            if (Button("Cancel")) {
                newBeybladeName[0] = '\0';
                currentPopup = PopupState::NONE;
                CloseCurrentPopup();
            }
            EndPopup();
        }
    }

    // New Stadium Popup
    if (currentPopup == PopupState::NEW_STADIUM) {
        drawNewItemPopup("New Stadium", "##StadiumName", newStadiumName, IM_ARRAYSIZE(newStadiumName),
            [&](const std::string& name) {
                if (!profile->createStadium(name)) return false;
                profile->setActiveStadium(profile->getAllStadiums().back()->getId());
                return true;
            });
    }

    // Confirm Profile Deletion Popup
    if (currentPopup == PopupState::DELETE_PROFILE) {
        drawConfirmDeletionPopup(currentProfileName, "Confirm Profile Deletion", [&]() {
            return pm.deleteProfile(profile->getId());
            });
    }

    // Confirm Beyblade Deletion Popup
    if (currentPopup == PopupState::DELETE_BEYBLADE) {
        drawConfirmDeletionPopup(currentBeybladeName, "Confirm Beyblade Deletion", [&]() {
            return profile->deleteBeyblade(beyblade->getId());
            });
    }

    // Confirm Stadium Deletion Popup
    if (currentPopup == PopupState::DELETE_STADIUM) {
        drawConfirmDeletionPopup(currentStadiumName, "Confirm Stadium Deletion", [&]() {
            return profile->deleteStadium(stadium->getId());
            });
    }
}