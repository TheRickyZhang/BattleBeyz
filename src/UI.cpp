////////////////////////////////////////////////////////////////////////////////
// UI.cpp -- User Interface Code -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "UI.h"

// TODO: Make more helper functions for UI, especially as it gets more complex

// Centers and wraps text in window
inline void CenterWrappedText(float window_center_x, float wrap_width, const char* text) {
    ImVec2 textSize = ImGui::CalcTextSize(text, text + strlen(text), false, wrap_width);
    ImGui::SetCursorPosX(window_center_x - textSize.x / 2.0f);
    ImGui::PushTextWrapPos(window_center_x + wrap_width / 2.0f);
    ImGui::TextWrapped(text);
    ImGui::PopTextWrapPos();
}

// Calculates the size of a button based on text and padding
ImVec2 CalculateButtonSize(const char* text, float padding = 20.0f, float height = 30.0f) {
    ImVec2 textSize = ImGui::CalcTextSize(text);
    return ImVec2(textSize.x + padding, height);
}

// Centers a button and returns true if clicked
inline bool CenterButton(float window_center_x, const char* label, float padding = 20.0f, float height = 30.0f) {
    ImVec2 buttonSize = CalculateButtonSize(label, padding, height);
    ImGui::SetCursorPosX(window_center_x - buttonSize.x / 2.0f);
    return ImGui::Button(label, buttonSize);
}

inline void TextWithLink(const char* text, const char* url) {
    ImVec4 linkColor = ImVec4(0.2f, 0.2f, 1.0f, 1.0f);
    ImGui::TextColored(linkColor, text);

    // Check if the item is hovered and clicked
    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (ImGui::IsMouseClicked(0)) {
            ShellExecute(nullptr, nullptr, url, nullptr, nullptr, SW_SHOW);
        }

        // Underline the text to indicate a hyperlink
        ImVec2 textSize = ImGui::CalcTextSize(text);
        ImVec2 cursorPos = ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y);
        ImGui::GetWindowDrawList()->AddLine(
                cursorPos,
                ImVec2(cursorPos.x + textSize.x, cursorPos.y),
                ImGui::GetColorU32(linkColor));
    }
}

// Render a red "Exit" button
inline bool RenderExitButton(GLFWwindow* window, const ImVec4& buttonColor = ImVec4(0.8f, 0.0f, 0.0f, 1.0f),
    const ImVec4& buttonHovered = ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
    const ImVec4& buttonActive = ImVec4(0.6f, 0.0f, 0.0f, 1.0f)) {
    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActive);
    bool exitPressed = ImGui::Button("Exit");
    if (exitPressed) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    ImGui::PopStyleColor(3);
    return exitPressed;
}


void setupBackground(GLFWwindow *window, Texture &backgroundTexture) {
    auto *data = static_cast<GameControl *>(glfwGetWindowUserPointer(window));
    auto windowWidth = *data->windowWidth;
    auto windowHeight = *data->windowHeight;
    auto backgroundShader = data->backgroundShader;
    auto quadRenderer = data->quadRenderer;

    if (backgroundTexture.ID != 0) {
        glm::mat4 ortho = glm::ortho(0.0f, (float) windowWidth, 0.0f, (float) windowHeight, -1.0f, 1.0f);
        backgroundShader->use();
        backgroundShader->setUniform1f("time", (float)glfwGetTime());
        // Bind the background texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture.ID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        backgroundShader->setInt("backgroundTexture", 0);
        quadRenderer->render();
    }
}

// TODO: Any way to make this logic easier to read?
void showHomeScreen(GLFWwindow *window, Texture &homeScreenTexture, Texture &backgroundTexture) {
    auto *data = static_cast<GameControl *>(glfwGetWindowUserPointer(window));
    auto windowWidth = data->windowWidth;
    auto windowHeight = data->windowHeight;
    int img_width = 400;
    int img_height = 400;

    setupBackground(window, backgroundTexture);

    glBindTexture(GL_TEXTURE_2D, homeScreenTexture.ID);

    // Calculate the size required to fit all elements plus some padding
    ImVec2 padding = ImVec2(50.0f, 50.0f);
    ImVec2 textSize = ImGui::CalcTextSize("Welcome to BattleBeyz!");
    ImVec2 imageSize = ImVec2((float)img_width, (float)img_height);

    const char* buttonText[] = { "Start Game", "Customize Beyblade", "About" };
    int buttonCount = sizeof(buttonText) / sizeof(buttonText[0]);

    // Calculate the maximum width of the buttons
    float maxButtonWidth = 0.0f;
    for (int i = 0; i < buttonCount; i++) {
        ImVec2 buttonSize = CalculateButtonSize(buttonText[i]);
        if (buttonSize.x > maxButtonWidth) {
            maxButtonWidth = buttonSize.x;
        }
    }

    // Calculate total height required
    float totalHeight = textSize.y + 20 + imageSize.y + 20 + buttonCount * 40 + padding.y * 2;

    // Calculate total width required
    float totalWidth = std::max({ textSize.x, imageSize.x, maxButtonWidth }) + padding.x * 2;

    ImGui::SetNextWindowSize(ImVec2(totalWidth, totalHeight));
    ImGui::SetNextWindowPos(ImVec2((*windowWidth - totalWidth) / 2.0f, (*windowHeight - totalHeight) / 2.0f));
    ImGui::Begin("Home Screen", nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoTitleBar);

    // Centering helper
    float window_center_x = totalWidth / 2.0f;

    // Center the text at the top
    ImGui::SetCursorPos(ImVec2(window_center_x - textSize.x / 2.0f, padding.y));
    ImGui::Text("Welcome to BattleBeyz!");

    // Center the image below the text
    if (homeScreenTexture.ID != 0) {
        ImGui::SetCursorPos(ImVec2(window_center_x - img_width / 2.0f, padding.y + textSize.y + 20));
        ImGui::Image((void *) (intptr_t) homeScreenTexture.ID, ImVec2((float)img_width, (float)img_height));
    }

    // Center the buttons below the image
    float button_start_y = padding.y + textSize.y + 20 + img_height + 20;

    for (int i = 0; i < buttonCount; i++) {
        ImVec2 buttonSize = CalculateButtonSize(buttonText[i]);

        ImGui::SetCursorPos(ImVec2(window_center_x - buttonSize.x / 2.0f, button_start_y + i * 40));
        if (ImGui::Button(buttonText[i], buttonSize)) {
            if (i == 0) {
                data->showHomeScreen = false;
                data->showInfoScreen = true;
            } else if (i == 1) {
                data->showCustomizeScreen = true;
            } else if (i == 2) {
                data->showAboutScreen = true;
            }
        }
    }

    ImGui::End();
}

void showOptionsScreen(GLFWwindow* window) {
    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Set the window size (e.g., 300x200) and calculate its centered position
    ImVec2 optionsWindowSize = ImVec2(300, 200);
    ImVec2 optionsWindowPos = ImVec2((windowWidth - optionsWindowSize.x) / 2, (windowHeight - optionsWindowSize.y) / 2);

    // Set window position and size, and disable moving
    ImGui::SetNextWindowPos(optionsWindowPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(optionsWindowSize);

    ImGui::Begin("Options", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    ImVec2 windowCenter = ImGui::GetWindowSize();
    float windowCenterX = windowCenter.x / 2.0f;

    if (CenterButton(windowCenterX, "Settings")) {
        // TODO: Add settings functionality
    }

    if (CenterButton(windowCenterX, "Home")) {
        data->showHomeScreen = true;
        data->showOptionsScreen = false;
    }

    // Reuse RenderExitButton without color options
    RenderExitButton(window);

    ImGui::End();
}



void showInfoScreen(GLFWwindow *window, float (*imguiColor)[3]) {
    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings and Launch Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Pause button
    ImGui::SameLine();
    if (ImGui::Button(data->currentState == ProgramState::PAUSED ? "Resume" : "Pause")) {
        data->currentState = (data->currentState == ProgramState::PAUSED) ? ProgramState::ACTIVE : ProgramState::PAUSED;
    }

    // Exit button
    ImGui::SameLine();
    RenderExitButton(window);

    // Home Button
    ImGui::SameLine();
    if (ImGui::Button("Home")) {
        data->showHomeScreen = true;
        data->showInfoScreen = false;
    }

    ImGui::Separator();

    std::vector<Beyblade*> beyblades = data->physicsWorld->getBeyblades();

    for (int i = 0; i < beyblades.size(); ++i) {
        BeybladeBody* beybladeBody = beyblades[i]->getRigidBody();

        // Each Beyblade has its own collapsible header that can be expanded/collapsed independently
        if (ImGui::CollapsingHeader(beyblades[i]->getName().data())) {
            ImGui::Text("Initial Velocity");
            glm::vec3 initialVelocity = beybladeBody->getVelocity();
            ImGui::SliderFloat("Velocity X", &initialVelocity.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Velocity Y", &initialVelocity.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Velocity Z", &initialVelocity.z, -100.0f, 100.0f);

            ImGui::Text("Initial Center");
            glm::vec3 initialCenter = beybladeBody->getCenter();
            ImGui::SliderFloat("Center X", &initialCenter.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Center Y", &initialCenter.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Center Z", &initialCenter.z, -100.0f, 100.0f);

            ImGui::Text("Initial Angular Velocity");
            glm::vec3 initialAngularVelocity = beybladeBody->getAngularVelocity();
            ImGui::SliderFloat("Angular Velocity X", &initialAngularVelocity.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Angular Velocity Y", &initialAngularVelocity.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Angular Velocity Z", &initialAngularVelocity.z, -100.0f, 100.0f);

            if (ImGui::Button("Apply Launch Settings")) {
                beybladeBody->setInitialLaunch(initialCenter, initialVelocity, initialAngularVelocity);
            }
        }
    }



    if (ImGui::Button("Launch")) {
        
    }

    // Color editor
    ImGui::ColorEdit3("background color", *imguiColor);

    // Checkbox to toggle showCamera
    ImGui::Checkbox("Bound Camera", &data->boundCamera);

    ImGui::End();
}

void showCustomizeScreen(GLFWwindow *window, Texture &backgroundTexture) {
    auto *data = static_cast<GameControl *>(glfwGetWindowUserPointer(window));
    auto windowWidth = data->windowWidth;
    auto windowHeight = data->windowHeight;

    setupBackground(window, backgroundTexture);

    // Centering helper
    float window_center_x = *windowWidth / 2.0f;

    ImGui::SetNextWindowSize(ImVec2((float)*windowWidth, (float)*windowHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Customize Screen", nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoTitleBar);

    // Center the text at the top
    ImGui::SetCursorPos(ImVec2(window_center_x - ImGui::CalcTextSize("Customize Your Beyblade!").x / 2.0f, 20));
    ImGui::Text("Customize Your Beyblade!");

    // Center the buttons below the image
    float button_start_y = 80;
    ImGui::SetCursorPos(ImVec2(window_center_x - 75, button_start_y));
    if (ImGui::Button("Save Customization", ImVec2(150, 30))) {
        // Add logic to save customization
    }

    ImGui::SetCursorPos(ImVec2(window_center_x - 75, button_start_y + 40));
    if (ImGui::Button("Back to Home", ImVec2(150, 30))) {
        data->showCustomizeScreen = false;
    }

    ImGui::End();
}

void showAboutScreen(GLFWwindow* window, Texture& backgroundTexture) {
    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));
    auto windowWidth = float(*data->windowWidth);
    auto windowHeight = float(*data->windowHeight);
    auto defaultFont = data->defaultFont;
    auto titleFont = data->titleFont;

    setupBackground(window, backgroundTexture);

    // Calculate the center of the window for text alignment and set wrapping width to 60% of window width
    float window_center_x = windowWidth / 2.0f;
    float wrap_width = windowWidth * 0.7f;

    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("About Screen", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

    // Apply larger font for the title and center it
    ImGui::PushFont(titleFont);
    CenterWrappedText(window_center_x, wrap_width, "About BattleBeyz");
    ImGui::PopFont();
    ImGui::Spacing();
    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    ImGui::PushFont(defaultFont);

    // Center the wrapped text blocks with a link
    {
        const char* part1 = "This project is currently in development. If you want to contribute, feel free to fork the repo";
        const char* linkText = "here";
        ImVec2 part1Size = ImGui::CalcTextSize(part1);
        ImVec2 linkSize = ImGui::CalcTextSize(linkText);
        float totalWidth = part1Size.x + linkSize.x;

        ImGui::SetCursorPosX(window_center_x - totalWidth / 2.0f);
        ImGui::TextUnformatted(part1);
        ImGui::SameLine();
        TextWithLink(linkText, "https://github.com/TheRickyZhang/BattleBeyz");
        ImGui::Spacing();
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // Center the wrapped text blocks with another link
    {
        const char* part1 = "Like what you see? Show your support by getting some hand-crafted Lego Beyblades on";
        const char* linkText = "my website";
        ImVec2 part1Size = ImGui::CalcTextSize(part1);
        ImVec2 linkSize = ImGui::CalcTextSize(linkText);
        float totalWidth = part1Size.x + linkSize.x;

        ImGui::SetCursorPosX(window_center_x - totalWidth / 2.0f);
        ImGui::TextUnformatted(part1);
        ImGui::SameLine();
        TextWithLink(linkText, "https://www.brickbeyz.com/shop");
        ImGui::Spacing();
    }

    CenterWrappedText(window_center_x, wrap_width, "For any inquiries, please contact me at rickyzhang196@outlook.com.");
    ImGui::Spacing();
    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    CenterWrappedText(window_center_x, wrap_width, "Thank you for playing BattleBeyz!");
    ImGui::Spacing();

    CenterWrappedText(window_center_x, wrap_width, "(c) 2024 Ricky Zhang. All rights reserved.");
    ImGui::PopFont();
    ImGui::Spacing();

    if (CenterButton(window_center_x, "Back to Home")) {
        data->showAboutScreen = false;
    }

    ImGui::End();
}

void showLoadingScreen(GLFWwindow* window, Texture& backgroundTexture, const char* message) {
    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));
    auto windowWidth = float(*data->windowWidth);
    auto windowHeight = float(*data->windowHeight);

    // Setup background
    setupBackground(window, backgroundTexture);

    // Set the size and position of the loading window
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Loading Screen", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

    // Center the loading message
    ImVec2 textSize = ImGui::CalcTextSize(message);
    ImGui::SetCursorPos(ImVec2((windowWidth - textSize.x) / 2.0f, (windowHeight - textSize.y) / 2.0f));
    ImGui::Text(message);

    ImGui::End();
}