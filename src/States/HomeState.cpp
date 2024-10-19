#include "HomeState.h"
#include "../GameEngine.h"
#include "../UI.h"
#include "StateIdentifiers.h"
#include <algorithm>

void HomeState::init() {}

void HomeState::cleanup() {}

void HomeState::pause() {}

void HomeState::resume() {}

void HomeState::handleEvents(GameEngine* game) {
    // Handle input for home state
}

void HomeState::update(GameEngine* game, float deltaTime) {}

void HomeState::draw(GameEngine* game) {
    int windowWidth = game->windowWidth;
    int windowHeight = game->windowHeight;

    renderBackground(game, "defaultBackground");

    std::vector<std::string> buttonText = { "Start Game", "Customize Beyblades", "About" };
    renderWindowWithButtons(game, "Main Menu", buttonText, "Battlebeyz!");

    ImGui::End();
}
