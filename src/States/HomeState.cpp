#include <algorithm>

#include "UI.h"
#include "GameEngine.h"
#include "HomeState.h"
#include "StateIdentifiers.h"

using namespace ImGui;

void HomeState::init() {}

void HomeState::cleanup() {}

void HomeState::pause() {}

void HomeState::resume() {}

void HomeState::handleEvents() {
    // Handle input for home state
}

void HomeState::update(float deltaTime) {}

void HomeState::draw() {
    // Note: ButtonStateMap is defined in GameState.h.
    static std::vector<std::string> buttonText = { "Start Game", "Profiles & Beyblades", "Settings", "About" };
    int windowWidth = game->windowWidth;
    int windowHeight = game->windowHeight;

    renderBackground(game, "defaultBackground");

    // TODO: Render pictures
    //// Define padding and image size
    //float padding = 50.0f;
    //ImVec2 imageSize = ImVec2(400.0f, 400.0f);

    //SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    //SetNextWindowPos(ImVec2(0.0f, 0.0f));
    //Begin("Home Screen", nullptr, MinimalWindow);

    //// Render the centered image
    //if (homeScreenTexture.ID != 0) {
    //    ImVec2 imagePos((*windowWidth - imageSize.x) / 2.0f, padding);
    //    SetCursorPos(imagePos);
    //    Image((void*)(intptr_t)homeScreenTexture.ID, imageSize);
    //}

    renderWindowWithButtons(game, "Main Menu", buttonText, "Battlebeyz!");

    ImGui::End();
}