#include "PauseState.h"
#include "../GameEngine.h"

void PauseState::init() {}

void PauseState::cleanup() {}

void PauseState::pause() {}

void PauseState::resume() {}

void PauseState::handleEvents(GameEngine* game) {
    if (ImGui::Button("Resume")) {
        game->popState();
    }
}

void PauseState::update(GameEngine* game, float deltaTime) {}

void PauseState::draw(GameEngine* game) {
    ImGui::Begin("Paused");
    if (ImGui::Button("Resume")) {
        game->popState();
    }
    if (ImGui::Button("Quit to Main Menu")) {
        game->changeState(GameStateType::HOME);
    }
    ImGui::End();
}
