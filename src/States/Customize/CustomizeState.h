#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <future>

#include <imgui.h>

#include "DefaultValues.h"

#include "GameState.h"
#include "BeybladeBody.h"
#include "MeshData.h"
#include "StadiumPreview.h"

class Profile;
class Beyblade;
class Stadium;

class CustomizeState : public GameState {
public:
    CustomizeState(GameEngine* _game) : GameState(_game) {};

    void init() override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvents() override;
    void onResize(int width, int height) override;
    void update(float deltaTime) override;
    void draw() override;


    GameStateType getStateType() const override { return GameStateType::CUSTOMIZE; }

private:
    // A multi-MB .obj takes tens of milliseconds to parse, so it runs on the job system
    // and the resulting geometry is uploaded here on the main thread once it lands.
    std::future<MeshData> pendingMesh;
    std::shared_ptr<Beyblade> meshImportTarget;
    std::string meshImportPath;

    float leftTextWidth, rightButton1Width, rightButton2Width;  // Static, initialized in init()
    float rightButton1X, rightButton2X, dropdownLeftX, dropdownWidth; // Change during onResize();

    enum class PopupState {
        NONE,
        NEW_PROFILE,
        NEW_BEYBLADE,
        NEW_STADIUM,
        DELETE_PROFILE,
        DELETE_BEYBLADE,
        DELETE_STADIUM
    };

    // Helper Methods

    void initializeData(std::vector<std::shared_ptr<Profile>>& profiles, std::shared_ptr<Profile>& profile,
        std::vector<std::shared_ptr<Beyblade>>& beyblades, std::shared_ptr<Beyblade>& beyblade,
        std::vector<std::shared_ptr<Stadium>>& stadiums, std::shared_ptr<Stadium>& stadium);

    std::shared_ptr<Profile> drawProfileSection(
        const std::vector<std::shared_ptr<Profile>>& profiles, const std::shared_ptr<Profile>& activeProfile
    );

    std::shared_ptr<Beyblade> drawBeybladeSection(
        const std::vector<std::shared_ptr<Beyblade>>& beyblades, const std::shared_ptr<Beyblade>& activeBeyblade,
        const std::shared_ptr<Profile>& profile
    );

    std::shared_ptr<Stadium> drawStadiumSection(
        const std::vector<std::shared_ptr<Stadium>>& stadiums, const std::shared_ptr<Stadium>& activeStadium,
        const std::shared_ptr<Profile>& profile
    );

    void drawManualCustomizeSection(std::shared_ptr<Beyblade> beyblade);
    void drawTemplateCustomizeSection(std::shared_ptr<Beyblade> beyblade);
    void drawStadiumCustomizeSection(std::shared_ptr<Stadium> stadium);

    void drawPopups(const std::shared_ptr<Profile>& profile, const std::shared_ptr<Beyblade>& beyblade, const std::shared_ptr<Stadium>& stadium);

    // Member Variables
    PopupState currentPopup = PopupState::NONE;

    // Temporary names
    char newProfileName[32] = "";
    char newBeybladeName[32] = "";
    char newStadiumName[32] = "";
    std::string currentProfileName = "";
    std::string currentBeybladeName = "";
    std::string currentStadiumName = "";

    // Temporary Beyblade Variables
    bool isTemplate = false;
    BeybladeBody* prevbladeBody = nullptr;
    Stadium* prevStadium = nullptr;
    int tempSelectedLayer = -1;
    int tempSelectedDisc = -1;
    int tempSelectedDriver = -1;

    // Stadium
    std::unique_ptr<StadiumPreview> stadiumPreview = nullptr;


    // Note: Must be in header file if using templates!
    template <typename T>
    std::shared_ptr<T> drawSection(const std::string& sectionName, const std::string& comboId,
        const std::vector<std::shared_ptr<T>>& items, const std::shared_ptr<T>& activeItem,
        std::function<void(const std::shared_ptr<T>&)> setActiveItem, std::function<std::string(const std::shared_ptr<T>&)> getItemName,
        std::function<void()> onCreateNew, std::function<void()> onDelete) {

        std::shared_ptr<T> updatedItem = activeItem;

        // Text label for the section
        ImGui::Text("%s", sectionName.c_str());
        ImGui::SameLine();
        ImGui::SetCursorPosX(dropdownLeftX);    // Use class member
        ImGui::SetNextItemWidth(dropdownWidth); // Use class member

        if (ImGui::BeginCombo(comboId.c_str(), activeItem ? getItemName(activeItem).c_str() : "None")) {
            for (const auto& item : items) {
                bool isSelected = (item == activeItem);
                if (ImGui::Selectable(getItemName(item).c_str(), isSelected)) {
                    updatedItem = item;
                    setActiveItem(item);
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // Create New Button
        ImGui::SameLine();
        ImGui::SetCursorPosX(rightButton1X); // Use class member
        if (ImGui::Button(("Create New##" + sectionName).c_str())) {
            onCreateNew();
        }

        // Delete Button
        if (updatedItem) {
            ImGui::SameLine();
            ImGui::SetCursorPosX(rightButton2X); // Use class member
            if (ImGui::Button(("Delete##" + sectionName).c_str())) {
                onDelete();
            }
        }

        return updatedItem;
    }
};