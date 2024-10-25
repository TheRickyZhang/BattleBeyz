#include "AboutState.h"
#include "../GameEngine.h"
#include "StateIdentifiers.h"

void AboutState::init() {}

void AboutState::cleanup() {}

void AboutState::pause() {}

void AboutState::resume() {}

void AboutState::handleEvents() {}

void AboutState::update(float deltaTime) {}

void AboutState::draw() {
    ImGui::Begin("About");
    if (ImGui::Button("Back to Home")) {
        game->changeState(GameStateType::HOME);
    }
    ImGui::Text("This is the about screen.");
    ImGui::End();
}
