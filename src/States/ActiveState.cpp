#include "ActiveState.h"
#include "../GameEngine.h"
#include "StateIdentifiers.h"

void ActiveState::init() {}

void ActiveState::cleanup() {}

void ActiveState::pause() {}

void ActiveState::resume() {}

void ActiveState::handleEvents(GameEngine* game) {}

void ActiveState::update(GameEngine* game, float deltaTime) {}

void ActiveState::draw(GameEngine* game) {
    ImGui::Begin("About");
    if (ImGui::Button("Back to Home")) {
        game->changeState(GameStateType::HOME);
    }
    ImGui::Text("This is the about screen.");
    ImGui::End();
}
