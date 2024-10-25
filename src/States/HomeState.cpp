#include "../UI.h"
#include "../GameEngine.h"
#include "HomeState.h"
#include "StateIdentifiers.h"
#include <algorithm>

void HomeState::init() {}

void HomeState::cleanup() {}

void HomeState::pause() {}

void HomeState::resume() {}

void HomeState::handleEvents() {
    // Handle input for home state
}

void HomeState::update(float deltaTime) {}

void HomeState::draw() {
    int windowWidth = game->windowWidth;
    int windowHeight = game->windowHeight;

    renderBackground(game, "defaultBackground");

    std::vector<std::string> buttonText = { "Start Game", "Customize Beyblades", "About" };
    renderWindowWithButtons(game, "Main Menu", buttonText, "Battlebeyz!");

    ImGui::End();
}
