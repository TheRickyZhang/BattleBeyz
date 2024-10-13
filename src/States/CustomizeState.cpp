#include "CustomizeState.h"
#include "../GameEngine.h"
#include "StateIdentifiers.h"

void CustomizeState::init() {}

void CustomizeState::cleanup() {}

void CustomizeState::pause() {}

void CustomizeState::resume() {}

void CustomizeState::handleEvents(GameEngine* game) {}

void CustomizeState::update(GameEngine* game, float deltaTime) {}

void CustomizeState::draw(GameEngine* game) {
    ImGui::Begin("Customize Menu");
    if (ImGui::Button("Back to Home")) {
        game->changeState(GameStateType::HOME);
    }
    // Add customization options here
    ImGui::End();
}
