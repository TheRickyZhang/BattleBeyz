#include "PauseState.h"
#include "GameEngine.h"

void PauseState::init() {}

void PauseState::cleanup() {}

void PauseState::pause() {}

void PauseState::resume() {}

void PauseState::handleEvents() {
    if (ImGui::Button("Resume")) {
        game->popState();
    }
}

void PauseState::update(float deltaTime) {}

void PauseState::draw() {
    ImGui::Begin("Paused");
    if (ImGui::Button("Resume")) {
        game->popState();
    }
    if (ImGui::Button("Quit to Main Menu")) {
        game->changeState(StateFactory::createState(game, GameStateType::HOME));
    }
    ImGui::End();
}
