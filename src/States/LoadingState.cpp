#include "LoadingState.h"
#include "../GameEngine.h"
#include "../UI.h"

void LoadingState::init() {}

void LoadingState::cleanup() {}

void LoadingState::pause() {}

void LoadingState::resume() {}

void LoadingState::handleEvents(GameEngine* game) {}

void LoadingState::update(GameEngine* game, float deltaTime) {
    progress += deltaTime;
    if (progress >= duration) {
        game->changeState(GameStateType::HOME);
    }
}

void LoadingState::draw(GameEngine* game) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(game->getWindow(), &windowWidth, &windowHeight);

    // Render the background first
    auto backgroundShader = game->backgroundShader;
    auto quadRenderer = game->quadRenderer;
    if (game->backgroundTexture->ID != 0) {
        glm::mat4 ortho = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);
        backgroundShader->use();
        backgroundShader->setUniform1f("time", (float)glfwGetTime());
        // Bind the background texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, game->backgroundTexture->ID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        backgroundShader->setInt("backgroundTexture", 0);
        quadRenderer->render();
    }

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
