#include "HomeState.h"
#include "../GameEngine.h"
#include "StateIdentifiers.h"

void HomeState::init() {}

void HomeState::cleanup() {}

void HomeState::pause() {}

void HomeState::resume() {}

void HomeState::handleEvents(GameEngine* game) {
    // Handle input for home state
}

void HomeState::update(GameEngine* game, float deltaTime) {}

void HomeState::draw(GameEngine* game) {
    ImGui::Begin("Main Menu");
    if (ImGui::Button("Start Game")) {
        game->changeState(GameStateType::ACTIVE);
    }
    if (ImGui::Button("Customize")) {
        game->changeState(GameStateType::CUSTOMIZE);
    }
    if (ImGui::Button("About")) {
        game->changeState(GameStateType::ABOUT);
    }
    ImGui::End();
}
