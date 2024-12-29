#include "GameState.h"
#include "GameEngine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void GameState::renderBackground(GameEngine* game, const std::string& textureName) {
    auto backgroundShader = game->backgroundShader;
    auto quadRenderer = game->quadRenderer;
    auto backgroundTexture = game->tm.getTexture(textureName);

    if (backgroundTexture && backgroundTexture->ID != 0) {
        glm::mat4 ortho = glm::ortho(0.0f, (float)game->windowWidth, 0.0f, (float)game->windowHeight, -1.0f, 1.0f);

        backgroundShader->use();
        backgroundShader->setFloat("time", (float)glfwGetTime());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture->ID);
        //backgroundShader->setInt("background", 0);

        quadRenderer->render(*backgroundShader); // Pass the shader to render
    }
}

void GameState::renderWindowWithButtons(GameEngine* game, const std::string& windowTitle,
    const std::vector<std::string>& buttonTexts,
    const std::string& beforeText, const std::string& afterText) {

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
    float totalWidth = std::max({ beforeTextSize.x, maxButtonWidth, afterTextSize.x }) + padding.x * 2;
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
