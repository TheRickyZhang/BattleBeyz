#include "UI.h"
#include "GameEngine.h"
#include "LoadingState.h"

using namespace ImGui;

void LoadingState::init() {
    loadingMessage = tips[(rand() * tips.size()) % tips.size()];
}

void LoadingState::cleanup() {
    loadingMessage = "";
}

void LoadingState::pause() {}
void LoadingState::resume() {}
void LoadingState::handleEvents() {}

void LoadingState::update(float deltaTime) {
    progress += deltaTime;
    if (progress >= duration) {
        game->changeState(GameStateType::HOME);
    }
}

void LoadingState::draw() {
    renderBackground(game, "defaultBackground");

    auto [centerX, wrapWidth] = SetWindowPositionAndSize(3, 4, 2, 2, 1, 2);

    Begin("Loading Screen", nullptr, MinimalWindow);

    PushFont(game->titleFont);
    centerWrappedText(centerX, wrapWidth, "Loading...");
    PopFont();

    centerWrappedText(centerX, wrapWidth, loadingMessage.c_str());

    centerProgressBar(centerX, 0.8f * wrapWidth, progress/duration, "Loading...");

    End();
}
