#include <memory>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GameState.h"

#include "ObjectShader.h"
#include "TextureManager.h"
#include "QuadRenderer.h"

using namespace std;
using namespace glm;
using namespace ImGui;

// Ensure that initStyleParams() is called in GameEngine::init();
float GameState::frameSpacingX = 0.0f;
float GameState::frameSpacingY = 0.0f;
float GameState::spacing = 0.0f;

void GameState::renderBackground(GameEngine* game, const string& textureName) {
    BackgroundShader* backgroundShader = game->backgroundShader;
    QuadRenderer* quadRenderer = game->quadRenderer;
    int width = game->windowWidth;
    int height = game->windowHeight;
    shared_ptr<Texture> backgroundTexture = game->tm.getTexture(textureName);

    glm::mat4 orthoProjection = glm::ortho(
        0.0f, static_cast<float>(game->windowWidth),
        0.0f, static_cast<float>(game->windowHeight),
        -1.0f, 1.0f
    );

    backgroundShader->setBackgroundGlobalParams(orthoProjection, 4.0f, (float)glfwGetTime());

    backgroundShader->setBackgroundObjectParams(scale(mat4(1.0f), vec3(width, height, 1.0f)), 0);

    // TOLOOK: Removing these causes background to not render until window is resized, even though it should be initialized with correct matrices.
    //glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(game->windowWidth, game->windowHeight, 1.0f));
    //game->quadRenderer->setModelMatrix(scale(mat4(1.0f), vec3(width, height, 1.0f)));
    quadRenderer->render(*backgroundShader, backgroundTexture);
}

void GameState::renderWindowWithButtons(GameEngine* game, const string& windowTitle,
    const vector<string>& buttonTexts,
    const string& beforeText, const string& afterText) {

    // Define padding and spacing
    const ImVec2 padding(50.0f, 50.0f);
    const float elementSpacing = 20.0f; // Space between elements
    const float buttonHeight = 40.0f;

    // Calculate sizes of beforeText and afterText
    ImVec2 beforeTextSize = beforeText.empty() ? ImVec2(0.0f, 0.0f) : ImGui::CalcTextSize(beforeText.c_str());
    ImVec2 afterTextSize = afterText.empty() ? ImVec2(0.0f, 0.0f) : ImGui::CalcTextSize(afterText.c_str());

    // Calculate maximum button width
    float maxButtonWidth = 0.0f;
    for (const auto& btnText : buttonTexts) {
        ImVec2 btnSize = ImGui::CalcTextSize(btnText.c_str());
        maxButtonWidth = std::max(maxButtonWidth, btnSize.x + 20.0f); // Adding some padding
    }

    // Calculate total width and height required for the window
    float totalWidth = max({ beforeTextSize.x, maxButtonWidth, afterTextSize.x }) + padding.x * 2;
    float totalHeight = padding.y * 2; // Start with top and bottom padding

    if (!beforeText.empty()) {
        totalHeight += beforeTextSize.y + elementSpacing;
    }

    if (!buttonTexts.empty()) {
        totalHeight += buttonTexts.size() * (buttonHeight + elementSpacing);
    }

    if (!afterText.empty()) {
        totalHeight += afterTextSize.y + elementSpacing;
    }

    // Set the window size and position to center it on the screen
    ImGui::SetNextWindowSize(ImVec2(totalWidth, totalHeight));
    ImGui::SetNextWindowPos(ImVec2((game->windowWidth - totalWidth) / 2.0f,
                                   (game->windowHeight - totalHeight) / 2.0f));

    // Begin the window
    ImGui::Begin(windowTitle.c_str(), nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar);

    // Calculate the center X position within the window
    float windowCenterX = totalWidth / 2.0f;

    // Initial Y position starts after top padding
    float currentY = padding.y;

    // Render beforeText if it exists
    if (!beforeText.empty()) {
        // Center the beforeText
        float textOffsetX = windowCenterX - beforeTextSize.x / 2.0f;
        ImGui::SetCursorPos(ImVec2(textOffsetX, currentY));
        ImGui::TextWrapped("%s", beforeText.c_str());
        currentY += beforeTextSize.y + elementSpacing;
    }

    // Render buttons
    for (size_t i = 0; i < buttonTexts.size(); ++i) {
        const auto& btnText = buttonTexts[i];
        float buttonWidth = maxButtonWidth;
        ImVec2 buttonSize(buttonWidth, buttonHeight);

        // Center the button
        float buttonOffsetX = windowCenterX - buttonWidth / 2.0f;
        ImGui::SetCursorPos(ImVec2(buttonOffsetX, currentY));

        if (ImGui::Button(btnText.c_str(), buttonSize)) {
            // Handle button actions based on button index or text
            auto it = buttonStateMap.find(btnText);
            if (it != buttonStateMap.end()) {
                game->changeState(it->second);
            }
        }

        currentY += buttonHeight + elementSpacing;
    }

    // Render afterText if it exists
    if (!afterText.empty()) {
        // Center the afterText
        float textOffsetX = windowCenterX - afterTextSize.x / 2.0f;
        ImGui::SetCursorPos(ImVec2(textOffsetX, currentY));
        ImGui::TextWrapped("%s", afterText.c_str());
        currentY += afterTextSize.y + elementSpacing;
    }
}
