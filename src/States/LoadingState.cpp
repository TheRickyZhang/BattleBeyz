#include "../UI.h"
#include "../GameEngine.h"
#include "LoadingState.h"

void LoadingState::init() {}

void LoadingState::cleanup() {}

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
    int windowWidth, windowHeight;
    glfwGetWindowSize(game->getWindow(), &windowWidth, &windowHeight);

    // Render the background first
    renderBackground(game, "defaultBackground");

    // Begin a plain, non-collapsible center window
    ImGui::SetNextWindowPos(ImVec2(windowWidth / 2.0f, windowHeight / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 200));
    renderPlainScreen("LoadingState", windowWidth / 2.0f, windowHeight / 2.0f, 400.0f, 200.0f);

    ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("Loading...").x) / 2.0f);
    ImGui::Text("Loading...");

    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    float progressBarWidth = 300.0f;
    ImGui::SetCursorPosX((400 - progressBarWidth) / 2.0f);
    ImGui::ProgressBar(progress / duration, ImVec2(progressBarWidth, 0.0f));

    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("Please wait...").x) / 2.0f);
    ImGui::Text("Please wait...");

    ImGui::End();
}
