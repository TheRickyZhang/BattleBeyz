////////////////////////////////////////////////////////////////////////////////
// UI.cpp -- User Interface Code -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

// NEWUI Huge changes in this file.

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include "UI.h"

using namespace ImGui;

// TODO
// OK does not save -- should it?
//
// How to get error reasons during profile processing?
//
// Dropdown lists are difficult to read. Need different background or border.

const int MAX_PROFILE_NAME_LENGTH = 30;

static int activeBlade = 0;                 // Blade 0 or Blade 1
static int activeBladeOld = 0;              // Previous value
static std::vector<int> activeProfileItem;  // Index for Which profile is active in each blade.
static GameControl* gameControl = nullptr;
static char newProfileName[MAX_PROFILE_NAME_LENGTH];
static std::vector<std::string> profileItems;  // Names of selectable profiles
static bool showPromptForProfile = false;

// These are the configuraable values; there's one item per possible blade.

static std::vector<float> discMass;
static std::vector<float> discMomentOfInertia;
static std::vector<float> driverCoefficientOfFriction;
static std::vector<float> driverMass;
static std::vector<float> driverMomentOfInertia;
static std::vector<float> layerCoefficientOfRestitution;
static std::vector<float> layerMass;
static std::vector<float> layerMomentOfInertia;
static std::vector<float> layerRecoilDistributionMean;
static std::vector<float> layerRecoilDistributionStdDev;

// Data for information screen

static std::string runState = "Idle";
bool runStateIsError = false;

// Centers and wraps text in window
void centerWrappedText(float windowCenterX, float wrapWidth, const char* text) {
    ImVec2 textSize = CalcTextSize(text, text + strlen(text), false, wrapWidth);
    SetCursorPosX(windowCenterX - textSize.x / 2.0f);
    PushTextWrapPos(windowCenterX + wrapWidth / 2.0f);
    TextWrapped(text);
    PopTextWrapPos();
}

// Calculates the size of a button based on text and padding
ImVec2 calculateButtonSize(const char* text, float padding = 20.0f, float height = 30.0f) {
    ImVec2 textSize = CalcTextSize(text);
    return ImVec2(textSize.x + padding, height);
}

void renderPlainScreen(const char* windowID, float windowCenterX, float windowCenterY, float width, float height) {
    SetNextWindowPos(ImVec2(windowCenterX, windowCenterY), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    SetNextWindowSize(ImVec2(width, height));
    Begin(windowID, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
}


// Centers a button and returns true if clicked
bool centerButton(float windowCenterX, const char* label, float padding = 20.0f, float height = 30.0f) {
    ImVec2 buttonSize = calculateButtonSize(label, padding, height);
    SetCursorPosX(windowCenterX - buttonSize.x / 2.0f);
    return Button(label, buttonSize);
}

void textWithLink(const char* text, const char* url) {
    ImVec4 linkColor = ImVec4(0.2f, 0.2f, 1.0f, 1.0f);
    TextColored(linkColor, text);

    // Check if the item is hovered and clicked
    if (IsItemHovered()) {
        SetMouseCursor(ImGuiMouseCursor_Hand);
        if (IsMouseClicked(0)) {
            ShellExecute(nullptr, nullptr, url, nullptr, nullptr, SW_SHOW);
        }

        // Underline the text to indicate a hyperlink
        ImVec2 textSize = CalcTextSize(text);
        ImVec2 cursorPos = ImVec2(GetItemRectMin().x, GetItemRectMax().y);
        GetWindowDrawList()->AddLine(
                cursorPos,
                ImVec2(cursorPos.x + textSize.x, cursorPos.y),
                GetColorU32(linkColor));
    }
}

// Render a red "Exit" button
bool renderExitButton(GLFWwindow* window, const ImVec4& buttonColor = ImVec4(0.8f, 0.0f, 0.0f, 1.0f),
    const ImVec4& buttonHovered = ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
    const ImVec4& buttonActive = ImVec4(0.6f, 0.0f, 0.0f, 1.0f)) {
    PushStyleColor(ImGuiCol_Button, buttonColor);
    PushStyleColor(ImGuiCol_ButtonHovered, buttonHovered);
    PushStyleColor(ImGuiCol_ButtonActive, buttonActive);
    bool exitPressed = Button("Exit");
    if (exitPressed) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    PopStyleColor(3);
    return exitPressed;
}

void showTooltip(const char* text) {
    if (IsItemHovered()) {
        SetTooltip("%s", text);
    }
}

bool sliderWithReset(const char* label, float* value, float min, float max, float defaultValue) {
    bool changed = SliderFloat(label, value, min, max);
    SameLine();
    if (Button("Reset")) {
        *value = defaultValue;
        changed = true;
    }
    return changed;
}

bool colorPickerWithReset(const char* label, ImVec4* color, const ImVec4& defaultColor) {
    bool changed = ColorEdit4(label, (float*)color);
    SameLine();
    if (Button("Reset")) {
        *color = defaultColor;
        changed = true;
    }
    return changed;
}

bool collapsingSectionWithToggle(const char* label, bool* isVisible) {
    bool open = CollapsingHeader(label);
    SameLine(GetWindowWidth() - 30);
    Checkbox("##Visible", isVisible);
    return open;
}

void centerColoredText(float windowCenterX, const ImVec4& color, const char* text) {
    SetCursorPosX(windowCenterX - CalcTextSize(text).x / 2);
    TextColored(color, "%s", text);
}

void DrawDiscreteFloatControl(const char* parameterText, float maxTextSize, const char* prefix, float& value, float minVal, float maxVal, float step, float stepFast, const char* format, std::function<void()> onModified) {
    float spacing = ImGui::GetStyle().ItemSpacing.x, padding = ImGui::GetStyle().WindowPadding.x;
    float availableWidth = ImGui::GetContentRegionAvail().x - maxTextSize - 2 * padding - 2 * spacing;
    float sliderWidth = availableWidth * 0.65f;
    float inputWidth = availableWidth * 0.35f;

    // Unique identifier from parameter text
    std::string sliderLabel = std::string("##slider_") + prefix + parameterText;
    std::string inputLabel = std::string("##input_") + prefix + parameterText;

    // [--Label--|------Slider------|--Input--]
    ImGui::BeginGroup();

    ImGui::Text(parameterText);
    ImGui::SameLine();
    ImGui::SetCursorPosX(maxTextSize + padding + spacing);

    ImGui::PushItemWidth(sliderWidth);
    int discrete = floatToDiscreteInt(value, minVal, maxVal);
    if (ImGui::SliderInt(sliderLabel.c_str(), &discrete, 1, 10)) {
        value = discreteIntToFloat(discrete, minVal, maxVal);
        onModified();
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushItemWidth(inputWidth);
    if (ImGui::InputFloat(inputLabel.c_str(), &value, step, stepFast, format, ImGuiInputTextFlags_CharsDecimal)) {
        value = std::clamp(value, minVal, maxVal);
        discrete = floatToDiscreteInt(value, minVal, maxVal);
        onModified();
    }
    ImGui::PopItemWidth();

    ImGui::EndGroup();
}


float getMaxWidth(const std::vector<std::string>& text) {
    float maxWidth = 0.0f;
    for (int i = 0; i < text.size(); i++) {
        ImVec2 textSize = CalcTextSize(text[i].c_str());
        if (textSize.x > maxWidth) {
            maxWidth = textSize.x;
        }
    }
    return maxWidth + 20.0f;  // Padding
}


static void setupBackground(GLFWwindow *window, Texture &backgroundTexture) {
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

/*
Result for (3, 4, 2, 1):          Result for (3, 4, 2, 1, 2, 3):
    +---+---+---+---+                +---+---+---+---+
    |   |   |   |   |  1             |   |   |   |   |  1
    +---+---+---+---+                +---+---+---+---+
    | W |   |   |   |  2             | W | W | W |   |  2
    +---+---+---+---+                +---+---+---+---+
    |   |   |   |   |  3             | W | W | W |   |  3
    +---+---+---+---+                +---+---+---+---+
      1   2   3   4                    1   2   3   4
*/
void SetWindowPositionAndSize(int numRows, int numCols, int row, int col, int rowSize, int colSize) {
    row--; col--;
    ImGuiIO& io = GetIO();

    // Calculate the size of each segment
    float windowWidth = io.DisplaySize.x / numCols;
    float windowHeight = io.DisplaySize.y / numRows;

    // Calculate position based on the grid
    float xPos = windowWidth * col;
    float yPos = windowHeight * row;

    SetNextWindowPos(ImVec2(xPos, yPos), ImGuiCond_Always);
    SetNextWindowSize(ImVec2(colSize * windowWidth, rowSize * windowHeight), ImGuiCond_Always);
}

void showAboutScreen(GLFWwindow* window, Texture& backgroundTexture)
{
    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));
    auto windowWidth = float(*data->windowWidth);
    auto windowHeight = float(*data->windowHeight);
    auto defaultFont = data->defaultFont;
    auto titleFont = data->titleFont;

    setupBackground(window, backgroundTexture);

    // Calculate the center of the window for text alignment and set wrapping width to 60% of window width
    float windowCenterX = windowWidth / 2.0f;
    float wrapWidth = windowWidth * 0.7f;

    //SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    //SetNextWindowPos(ImVec2(0, 0));
    SetWindowPositionAndSize(1, 1, 1, 1); // Take up whole screen
    Begin("About Screen", nullptr, MinimalWindow);

    // Apply larger font for the title and center it
    PushFont(titleFont);
    centerWrappedText(windowCenterX, wrapWidth, "About BattleBeyz");
    PopFont();
    Spacing();
    Dummy(ImVec2(0.0f, 20.0f));

    PushFont(defaultFont);

    // Center the wrapped text blocks with a link
    {
        const char* part1 = "This project is currently in development. If you want to contribute, feel free to fork the repo";
        const char* linkText = "here";
        ImVec2 part1Size = CalcTextSize(part1);
        ImVec2 linkSize = CalcTextSize(linkText);
        float totalWidth = part1Size.x + linkSize.x;

        SetCursorPosX(windowCenterX - totalWidth / 2.0f);
        TextUnformatted(part1);
        SameLine();
        textWithLink(linkText, "https://github.com/TheRickyZhang/BattleBeyz");
        Spacing();
    }

    Dummy(ImVec2(0.0f, 10.0f));

    // Center the wrapped text blocks with another link
    {
        const char* part1 = "Like what you see? Show your support by getting some hand-crafted Lego Beyblades on";
        const char* linkText = "my website";
        ImVec2 part1Size = CalcTextSize(part1);
        ImVec2 linkSize = CalcTextSize(linkText);
        float totalWidth = part1Size.x + linkSize.x;

        SetCursorPosX(windowCenterX - totalWidth / 2.0f);
        TextUnformatted(part1);
        SameLine();
        textWithLink(linkText, "https://www.brickbeyz.com/shop");
        Spacing();
    }

    centerWrappedText(windowCenterX, wrapWidth, "For any inquiries, please contact me at rickyzhang196@outlook.com.");
    Spacing();
    Dummy(ImVec2(0.0f, 20.0f));

    centerWrappedText(windowCenterX, wrapWidth, "Thank you for playing BattleBeyz!");
    Spacing();

    centerWrappedText(windowCenterX, wrapWidth, "(c) 2024 Ricky Zhang. All rights reserved.");
    PopFont();
    Spacing();

    if (centerButton(windowCenterX, "Back to Home")) {
        data->showAboutScreen = false;
    }

    End();
}

void showCustomizeScreen(GLFWwindow* window, Texture& backgroundTexture)
{
    extern int activeBlade;
    extern int activeBladeOld;
    extern char newProfileName[MAX_PROFILE_NAME_LENGTH];
    extern bool showPromptForProfile;

    extern std::vector<float> discMass;
    extern std::vector<float> discMomentOfInertia;
    extern std::vector<float> driverCoefficientOfRestitution;
    extern std::vector<float> driverMass;
    extern std::vector<float> driverMomentofInertia;
    extern std::vector<float> layerCoefficientOfRestitution;
    extern std::vector<float> layerMass;
    extern std::vector<float> layerMomentOfInertia;
    extern std::vector<float> layerRecoilDistributionMean;
    extern std::vector<float> layerRecoilDistributionStdDev;
    extern char profileName[2][MAX_PROFILE_NAME_LENGTH];

    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));
    auto windowWidth = data->windowWidth;
    auto windowHeight = data->windowHeight;

    setupBackground(window, backgroundTexture);

    // Centering helper
    float windowCenterX = *windowWidth / 2.0f;

    SetNextWindowSize(ImVec2((float)*windowWidth, (float)*windowHeight));
    SetNextWindowPos(ImVec2(0, 0));

    Begin("Customize Screen", nullptr, MinimalWindow);

    // Center the text at the top
    SetCursorPos(ImVec2(windowCenterX - CalcTextSize("Customize Your Beyblades!").x / 2.0f, 20));
    Text("Customize Your Beyblades!");

    // The ImGui API is very awkward -- To initialize this dialog data you need to click a button in some
    // parent window to run initialization code for here.  Yuck.

    RadioButton("Blade1", &activeBlade, 0);
    SameLine();
    RadioButton("Blade2", &activeBlade, 1);

    // Can't get a radio button changed event, so detect the change like this.
    if (activeBlade != activeBladeOld) {  // Reset this input box when you switch blades
        newProfileName[0] = '\0';
        activeBladeOld = activeBlade;
    }

    // Arrays for mass, radius, etc.

    PushItemWidth(600);  // Note: You can limit width of the sliders like this

    Text("Profile");
    if (BeginCombo("##empty", profileItems[activeProfileItem[activeBlade]].c_str())) {
        for (int i = 0; i < (int)profileItems.size(); i++) {
            bool isSelected = (activeProfileItem[activeBlade] == i);
            if (Selectable(profileItems[i].c_str(), isSelected)) {
                activeProfileItem[activeBlade] = i;
                UICustomScreenProfileSelected(i);
            }
            if (isSelected) {
                SetItemDefaultFocus();
            }
        }
        EndCombo();
    }

    SameLine();
    if (Button("Save Profile As...")) {
        newProfileName[0] = '\0';
        showPromptForProfile = true;
    }
    UIPromptForProfile();

    // NOTE: Moment of Inertia limits are in GRAMS here.  See Units.txt.

    NewLine();
    Text("Layer (top)");
    SliderFloat("Mass (g)##layer", &layerMass[activeBlade], 1.0f, 99.0f, "%.1f");
    SliderFloat("Moment of Inertia (g*m**2)##layer", &layerMomentOfInertia[activeBlade], 3.0e-3f, 1.6e-2f, "%.9f");
    SliderFloat("Coefficient of Restitution##layer", &layerCoefficientOfRestitution[activeBlade], 0.0f, 0.5f, "%.3f");
    // TODO: WHAT ARE THE RANGES FOR THESE?
    SliderFloat("Recoil Distribution Mean##layer", &layerRecoilDistributionMean[activeBlade], 0.1f, 1.0f, "%.3f");
    SliderFloat("Recoil Distribution StdDev##layer", &layerRecoilDistributionStdDev[activeBlade], 0.1f, 1.0f, "%.3f");

    Text("Disc (middle)");
    SliderFloat("Mass (g)##disc", &discMass[activeBlade], 1.0f, 99.0f, "%.1f");
    SliderFloat("Moment of Inertia (g*m**2)##disc", &discMomentOfInertia[activeBlade], 2.3e-3f, 2.025e-2f, "%.9f");

    Text("Driver (bottom)");
    SliderFloat("Mass (g)##driver", &driverMass[activeBlade], 1.0f, 99.0f, "%.1f");
    SliderFloat("Moment of Inertia (g*m**2)##driver", &driverMomentOfInertia[activeBlade], 7.5e-6f, 2.0e-5f, "%.9f");
    SliderFloat("Coefficient of Friction##driver", &driverCoefficientOfFriction[activeBlade], 0.1f, 0.6f, "%.3f");

    PopItemWidth();

    // Center the buttons below the image

    float button_start_y = 650.0f;
    SetCursorPos(ImVec2(windowCenterX - 150 - 20, button_start_y));
    if (Button("OK", ImVec2(150, 30))) {
        UICustomSettingsSave(data);
        data->showCustomizeScreen = false;
    }

    SetCursorPos(ImVec2(windowCenterX + 20, button_start_y));
    if (Button("Cancel", ImVec2(150, 30))) {
        data->showCustomizeScreen = false;
    }

    End();
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
    ImVec2 textSize = CalcTextSize("Welcome to BattleBeyz!");
    ImVec2 imageSize = ImVec2((float)img_width, (float)img_height);

    const char* buttonText[] = { "Start Game", "Customize Beyblades", "About" };
    int buttonCount = sizeof(buttonText) / sizeof(buttonText[0]);

    // Calculate the maximum width of the buttons
    float maxButtonWidth = 0.0f;
    for (int i = 0; i < buttonCount; i++) {
        ImVec2 buttonSize = calculateButtonSize(buttonText[i]);
        if (buttonSize.x > maxButtonWidth) {
            maxButtonWidth = buttonSize.x;
        }
    }

    // Calculate total height required
    float totalHeight = textSize.y + 20 + imageSize.y + 20 + buttonCount * 40 + padding.y * 2;

    // Calculate total width required
    float totalWidth = std::max({ textSize.x, imageSize.x, maxButtonWidth }) + padding.x * 2;

    SetNextWindowSize(ImVec2(totalWidth, totalHeight));
    SetNextWindowPos(ImVec2((*windowWidth - totalWidth) / 2.0f, (*windowHeight - totalHeight) / 2.0f));
    Begin("Home Screen", nullptr, MinimalWindow);

    // Centering helper
    float windowCenterX = totalWidth / 2.0f;

    // Center the text at the top
    SetCursorPos(ImVec2(windowCenterX - textSize.x / 2.0f, padding.y));
    Text("Welcome to BattleBeyz!");

    // Center the image below the text
    if (homeScreenTexture.ID != 0) {
        SetCursorPos(ImVec2(windowCenterX - img_width / 2.0f, padding.y + textSize.y + 20));
        Image((void *) (intptr_t) homeScreenTexture.ID, ImVec2((float)img_width, (float)img_height));
    }

    // Center the buttons below the image
    float button_start_y = padding.y + textSize.y + 20 + img_height + 20;

    for (int i = 0; i < buttonCount; i++) {
        ImVec2 buttonSize = calculateButtonSize(buttonText[i]);

        SetCursorPos(ImVec2(windowCenterX - buttonSize.x / 2.0f, button_start_y + i * 40));
        if (Button(buttonText[i], buttonSize)) {
            if (i == 0) {
                data->showHomeScreen = false;
                data->showInfoScreen = true;
                UISetRunState(false, "Running");
            }
            else if (i == 1) {
                data->showCustomizeScreen = true;
                UICustomScreenInit(data);
            }
            else if (i == 2) {
                data->showAboutScreen = true;
            }
        }
    }

    End();
}

/**
* Show game information.
* add ing the backgroundCOlor in form  float(*imguiColor)[3]
*/

void drawInfoScreen(GLFWwindow* window) {
    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));

    SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    Begin("Settings and Launch Menu", nullptr);

    // Pause button
    SameLine();
    if (Button(data->currentState == ProgramState::PAUSED ? "Resume" : "Pause")) {
        data->currentState = (data->currentState == ProgramState::PAUSED) ? ProgramState::ACTIVE : ProgramState::PAUSED;

        std::string status;

        switch (data->currentState) {
            case ProgramState::PAUSED:
                status = "Paused";
                break;
            case ProgramState::ACTIVE:
            case ProgramState::LOADING:
                status = "Running";
                break;
            default:
                status = "Idle";
                break;
        }
        UISetRunState(false, status);
    }

    // Exit button
    SameLine();
    renderExitButton(window);

    // Home Button
    SameLine();
    if (Button("Home")) {
        data->showHomeScreen = true;
        data->showInfoScreen = false;
        UISetRunState(false, "Idle");
    }

    Separator();

    if (runStateIsError) {
        TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", runState.c_str());
    }
    else {
        Text("%s", runState.c_str());
    }

    std::vector<Beyblade*> beyblades = data->physicsWorld->getBeyblades();

    for (int i = 0; i < beyblades.size(); ++i) {
        BeybladeBody* beybladeBody = beyblades[i]->getRigidBody();

        // TODO: Sliders look silly here.  Just display as text, but how to center it?

        // Each Beyblade has its own collapsible header that can be expanded/collapsed independently
        if (CollapsingHeader(beyblades[i]->getName().data())) {
            Text("Velocity");  // Was "Initial Velocity"
            glm::vec3 initialVelocity = beybladeBody->getVelocity();
            SliderFloat("X##V", &initialVelocity.x, -100.0f, 100.0f);
            SliderFloat("Y##V", &initialVelocity.y, -100.0f, 100.0f);
            SliderFloat("Z##V", &initialVelocity.z, -100.0f, 100.0f);

            Text("Center");
            glm::vec3 initialCenter = beybladeBody->getCenter();
            SliderFloat("X##CTR", &initialCenter.x, -100.0f, 100.0f);
            SliderFloat("Y##CTR", &initialCenter.y, -100.0f, 100.0f);
            SliderFloat("Z##CTR", &initialCenter.z, -100.0f, 100.0f);

            Text("Angular Velocity");
            glm::vec3 initialAngularVelocity = beybladeBody->getAngularVelocity();
            SliderFloat("X##AV", &initialAngularVelocity.x, -100.0f, 100.0f);
            SliderFloat("Y##AV", &initialAngularVelocity.y, -100.0f, 100.0f);
            SliderFloat("Z##AV", &initialAngularVelocity.z, -100.0f, 100.0f);

            if (Button("Apply Launch Settings")) {
                beybladeBody->setInitialLaunch(initialCenter, initialVelocity, initialAngularVelocity);
            }
        }
    }

    if (Button("Launch")) {

    }

    // Color editor
    //ColorEdit3("background color", *imguiColor);

    // Checkbox to toggle showCamera
    Checkbox("Bound Camera", &data->boundCamera);

    End();
}

void showLoadingScreen(GLFWwindow* window, Texture& backgroundTexture, const char* message) {
    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));
    auto windowWidth = float(*data->windowWidth);
    auto windowHeight = float(*data->windowHeight);

    // Setup background
    setupBackground(window, backgroundTexture);

    // Set the size and position of the loading window
    SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    SetNextWindowPos(ImVec2(0, 0));
    Begin("Loading Screen", nullptr, MinimalWindow);

    // Center the loading message
    ImVec2 textSize = CalcTextSize(message);
    SetCursorPos(ImVec2((windowWidth - textSize.x) / 2.0f, (windowHeight - textSize.y) / 2.0f));
    Text(message);

    End();
}

void showOptionsScreen(GLFWwindow* window) {
    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Set the window size (e.g., 300x200) and calculate its centered position
    ImVec2 optionsWindowSize = ImVec2(300, 200);
    ImVec2 optionsWindowPos = ImVec2((windowWidth - optionsWindowSize.x) / 2, (windowHeight - optionsWindowSize.y) / 2);

    // Set window position and size, and disable moving
    SetNextWindowPos(optionsWindowPos, ImGuiCond_Always);
    SetNextWindowSize(optionsWindowSize);

    Begin("Options", nullptr, MinimalWindow);

    ImVec2 windowCenter = GetWindowSize();
    float windowCenterX = windowCenter.x / 2.0f;

    if (centerButton(windowCenterX, "Settings")) {
        // TODO: Add settings functionality
    }

    if (centerButton(windowCenterX, "Home")) {
        data->showHomeScreen = true;
        data->showOptionsScreen = false;
    }

    // Reuse renderExitButton without color options
    renderExitButton(window);

    End();
}

/**
* Initialize Custom Screen dialog before display.
* 
* This is called when you click the "customize" button.  We reset everthing to
* defaults.  This is currently might be overkill, but allows us in the future
* to restart the game, too.
*/

static void UICustomScreenInit(GameControl* gc)
{
    extern std::vector<int> activeProfileItem;
    int defaultProfileIndex = 0;
    extern GameControl* gameControl;
    extern  std::vector<std::string> profileItems;

    extern std::vector<float> discMass;
    extern std::vector<float> discMomentOfInertia;
    extern std::vector<float> driverCoefficientOfFriction;
    extern std::vector<float> driverMass;
    extern std::vector<float> driverMomentofInertia;
    extern std::vector<float> layerCoefficientOfRestitution;
    extern std::vector<float> layerMass;
    extern std::vector<float> layerMomentOfInertia;
    extern std::vector<float> layerRecoilDistributionMean;
    extern std::vector<float> layerRecoilDistributionStdDev;

    if (gameControl != nullptr) {
        return;  // Initialize only once
    }

    gameControl = gc;  // We'll need this later

    // At the moment this is a bit weird.  The incoming blade structures start with default values,
    // and we create profile structures also weith default values!

    if (gc->iniFile == nullptr) {
        // First time through here. Set up the ini file handling

        gc->iniFile = new mINI::INIFile("beyblade_profiles.ini");
        gc->iniData = new mINI::INIStructure();

        if (!gc->iniFile->read(*gc->iniData)) {
            gc->iniData->clear();  // Couldn't read, create a new setup
        }

        for (auto it = gc->iniData->begin(); it != gc->iniData->end(); it++) {
            auto const& section = it->first;
            profileItems.push_back(section);
        }

        // You MUST at least have a "Default" profile.

        if (!gc->iniData->has("Default")) {
            Disc disc;
            Driver driver;
            Layer layer;

            UIProfileCreateSectionFromDefaults("Default", disc, driver, layer);
            profileItems.clear();
            profileItems.push_back("Default");

            if (!gc->iniFile->write(*gc->iniData)) {
                std::cerr << "Failed to write profile file" << std::endl;  // TODO: Why did it fail?
            };
        }

        std::sort(profileItems.begin(), profileItems.end());  // Sort the items

        // Find the index of "Default"
        defaultProfileIndex = (int)std::distance(profileItems.begin(), std::find(profileItems.begin(), profileItems.end(), "Default"));

        // TODO: What if the mesh has other weird settings that aren't really
        // the default?
    }

    /*
    discMass.clear();
    discMomentOfInertia.clear();
    driverCoefficientOfFriction.clear();
    driverMass.clear();
    driverMomentOfInertia.clear();
    layerCoefficientOfRestitution.clear();
    layerMass.clear();
    layerMomentOfInertia.clear();
    layerRecoilDistributionMean.clear();
    layerRecoilDistributionStdDev.clear();
    */

    // There must be at least TWO blades!  You might have more in the future,
    // although the customization UI is probably incomplete.

    std::vector<Beyblade*> blades = gc->physicsWorld->getBeyblades();
    for (size_t i = 0; i < blades.size(); i++) {
        activeProfileItem.push_back(defaultProfileIndex);

        Beyblade* b = blades[i];
        BeybladeBody* bb = b->getRigidBody();

        discMass.push_back(bb->getDiscMass().value() * 1000.0f);
        driverMass.push_back(bb->getDriverMass().value() * 1000.0f);
        layerMass.push_back(bb->getLayerMass().value() * 1000.0f);

        discMomentOfInertia.push_back(bb->getDiscMomentOfInertia().value() * 1000.0f);
        driverMomentOfInertia.push_back(bb->getDriverMomentOfInertia().value() * 1000.0f);
        layerMomentOfInertia.push_back(bb->getLayerMomentOfInertia().value() * 1000.0f);

        driverCoefficientOfFriction.push_back(bb->getDriverCOF().value());
        layerCoefficientOfRestitution.push_back(bb->getLayerCOR().value());

        layerRecoilDistributionMean.push_back(bb->getLayerRecoilDistribution().getMean().value());
        layerRecoilDistributionStdDev.push_back(bb->getLayerRecoilDistribution().getStdDev().value());
    }
}

/**
* Prompt for a new profile name and save the profile when selected.
*/

static void UIPromptForProfile()
{
    extern char newProfileName[MAX_PROFILE_NAME_LENGTH];
    extern bool showPromptForProfile;

    if (showPromptForProfile) {
        OpenPopup("Save Profile##title");
    }

    // Create a popup window
    if (BeginPopupModal("Save Profile##title", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Display a text input field for the file name
        InputText("Profile Name##text", newProfileName, MAX_PROFILE_NAME_LENGTH);

        // Display buttons to confirm or cancel the input

        if (Button("OK", ImVec2(120, 0))) {
            if (newProfileName[0] != '\0') {  // [Re]write the profiles, and add or replace the screen data.
                UIProfileCreateSectionFromScreen(newProfileName);

                if (std::find(profileItems.begin(), profileItems.end(), newProfileName) == profileItems.end()) {
                    profileItems.push_back(newProfileName);
                }

                std::sort(profileItems.begin(), profileItems.end());  // Sort the items

                int newItemIndex = (int)std::distance(profileItems.begin(), std::find(profileItems.begin(), profileItems.end(), newProfileName));
                activeProfileItem[activeBlade] = newItemIndex;

                newProfileName[0] = '\0';
            }

            showPromptForProfile = false;
            CloseCurrentPopup();
        }

        SameLine();

        if (Button("Cancel", ImVec2(120, 0))) {
            showPromptForProfile = false;
            CloseCurrentPopup();
        }

        EndPopup();
    }
}

/**
* Create a new section (i.e., a blade profile) in the ini data.
* 
* @param profileName                [in] Section name such as "Default".
* 
* @param disc                       [in] A disc object.
* 
* @param driver                     [in] A driver object.
* 
* @param layer                      [in] A layer object.
*/

static void UIProfileCreateSectionFromDefaults(const char* profileName, Disc& disc, Driver& driver, Layer& layer)
{
    extern GameControl* gameControl;

    mINI::INIStructure& i = *gameControl->iniData;  // Easy-to-type reference

    /**
    * Create a new section from the active screen values.  Rewrites the ini file.
    */
    if (!gameControl->iniFile->write(*gameControl->iniData)) {
        std::cerr << "Failed to write profile file" << std::endl;  // TODO: Why did it fail?
    }
    // 12/14/24" TODO: Migrate mINI or some other file system? gameControl is outdated
    //(*gameControl->iniData)[profileName].set({
    //    { "discMass", std::to_string(disc.mass) },
    //    { "discMomentOfInertia", std::to_string(disc.momentOfInertia) },
    //    { "driverCoefficientOfFriction",  std::to_string(driver.coefficientOfFriction) },
    //    { "driverMass",  std::to_string(driver.mass) },
    //    { "driverMomentOfInertia", std::to_string(driver.momentOfInertia) },
    //    { "layerCoefficientOfRestitution", std::to_string(layer.coefficientOfRestitution) },
    //    { "layerMass",  std::to_string(layer.mass) },
    //    { "layerMomentOfInertia", std::to_string(layer.momentOfInertia) },
    //    { "layerRecoilDistributionMean", std::to_string(layer.recoilDistribution.getMean()) },
    //    { "layerRecoilDistributionStdDev", std::to_string(layer.recoilDistribution.getStdDev()) }
    //    });
}

/**
* Handle profile selection change.
*/

static void UICustomScreenProfileSelected(int selectedItemIndex)
{
    extern int activeBlade;
    extern std::vector<int> activeProfileItem;

    activeProfileItem[activeBlade] = selectedItemIndex;
    UIUpdateBladeFromProfile();  // Change all values on the screen.
}

/**
* Update game state message in the information screen.
*/

void UISetRunState(bool isError, const std::string& msg)
{
    extern std::string runState;
    extern bool runStateIsError;

    runState = msg;
    runStateIsError = isError;
}

/**
* Update the screen for the active blade from the active profile.
*/

static void UIUpdateBladeFromProfile()
{
    extern int activeBlade;
    extern std::vector<int> activeProfileItem;
    extern std::vector<std::string> profileItems;
    extern GameControl* gameControl;

    std::string profileName = profileItems[activeProfileItem[activeBlade]];

    extern std::vector<float> discMass;
    extern std::vector<float> discMomentOfInertia;
    extern std::vector<float> driverCoefficientOfFriction;
    extern std::vector<float> driverMass;
    extern std::vector<float> driverMomentofInertia;
    extern std::vector<float> layerCoefficientOfRestitution;
    extern std::vector<float> layerMass;
    extern std::vector<float> layerMomentOfInertia;
    extern std::vector<float> layerRecoilDistributionMean;
    extern std::vector<float> layerRecoilDistributionStdDev;

    mINI::INIStructure& i = *gameControl->iniData;  // Easy-to-type reference

    // Easier way to do this?  I want the map for this section, but C++ won't deduce from the reference.

    discMass[activeBlade] = (float)(atof(i[profileName]["discMass"].c_str()) * 1000.0);
    discMomentOfInertia[activeBlade] = (float)(atof(i[profileName]["discMomentOfInertia"].c_str()) * 1000.0);
    
    driverCoefficientOfFriction[activeBlade] = (float)atof(i[profileName]["driverCoefficientOfFriction"].c_str());
    driverMass[activeBlade] = (float)(atof(i[profileName]["driverMass"].c_str()) * 1000.0);
    driverMomentOfInertia[activeBlade] = (float)(atof(i[profileName]["driverMomentOfInertia"].c_str()) * 1000.0);

    layerCoefficientOfRestitution[activeBlade] = (float)atof(i[profileName]["layerCoefficientOfRestitution"].c_str());
    layerMass[activeBlade] = (float)(atof(i[profileName]["layerMass"].c_str()) * 1000.0);
    layerMomentOfInertia[activeBlade] = (float)(atof(i[profileName]["layerMomentOfInertia"].c_str()) * 1000.0);
    layerRecoilDistributionMean[activeBlade] = (float)atof(i[profileName]["layerRecoilDistributionMean"].c_str());
    layerRecoilDistributionStdDev[activeBlade] = (float)atof(i[profileName]["layerRecoilDistributionStdDev"].c_str());
}
