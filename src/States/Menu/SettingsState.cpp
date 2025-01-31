#include "SettingsState.h"
#include "GameEngine.h"
#include "StateIdentifiers.h"
#include "ImGuiUI.h"

using namespace ImGui;
using namespace std;

void SettingsState::init() {}
void SettingsState::cleanup() {}
void SettingsState::pause() {}
void SettingsState::resume() {}
void SettingsState::handleEvents() {}
void SettingsState::onResize(int width, int height) {}
void SettingsState::update(float deltaTime) {}

void SettingsState::draw() {
    SetWindowPositionAndSize(1, 1, 1, 1);
    ImGui::Begin("Settings Menu", nullptr, MinimalWindow);

    // Centered heading
    centerWrappedText(static_cast<float>(game->windowWidth) / 2, static_cast<float>(game->windowWidth) * 0.6f, "Settings");

    // Volume slider
    static float volume = 0.5f;
    ImGui::Text("Volume");
    ImGui::SliderFloat("##VolumeSlider", &volume, 0.0f, 1.0f, "%.2f");

    // Mute checkbox
    static bool isMuted = false;
    ImGui::Checkbox("Mute Audio", &isMuted);

    // Resolution dropdown (example options)
    static const char* resolutions[] = { "800x600", "1280x720", "1920x1080" };
    static int currentResolution = 1; // Default to 1280x720
    ImGui::Text("Resolution");
    if (ImGui::BeginCombo("##ResolutionCombo", resolutions[currentResolution])) {
        for (int i = 0; i < IM_ARRAYSIZE(resolutions); i++) {
            bool isSelected = (currentResolution == i);
            if (ImGui::Selectable(resolutions[i], isSelected))
                currentResolution = i;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Fullscreen checkbox
    static bool isFullscreen = false;
    ImGui::Checkbox("Fullscreen", &isFullscreen);

    // Save button
    if (ImGui::Button("Save Settings")) {
        // Placeholder: implement saving logics
    }

    ImGui::SameLine();
    // Close/Back button
    if (ImGui::Button("Close")) {
        game->changeState(StateFactory::createState(game, GameStateType::HOME));
        ImGui::End();
        return;
    }

    ImGui::End();
}
