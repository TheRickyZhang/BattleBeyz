////////////////////////////////////////////////////////////////////////////////
// UI.cpp -- User Interface Code -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

// NEWUI Huge changes in this file.

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include "UI.h"
#include "Utils.h"

using namespace std;
using namespace ImGui;

// Centers and wraps text in CURRENT IMGUI window.
void centerWrappedText(float centerX, float wrapWidth, const std::string& text)
{
    std::istringstream iss(text);
    std::string word, line;
    while (iss >> word)
    {
        float lineWidth = ImGui::CalcTextSize((line + (line.empty() ? "" : " ") + word).c_str()).x;
        if (!line.empty() && lineWidth > wrapWidth)
        {
            ImGui::SetCursorPosX(centerX - ImGui::CalcTextSize(line.c_str()).x * 0.5f);
            ImGui::TextUnformatted(line.c_str());
            line = word;
        }
        else line += (line.empty() ? "" : " ") + word;
    }
    if (!line.empty())
    {
        ImGui::SetCursorPosX(centerX - ImGui::CalcTextSize(line.c_str()).x * 0.5f);
        ImGui::TextUnformatted(line.c_str());
    }
}

void centerProgressBar(float centerX, float barWidth, float progress, const string& overlay)
{
    float progressBarWidth = barWidth;
    ImGui::SetCursorPosX(centerX - progressBarWidth * 0.5f);
    ImGui::ProgressBar(progress, ImVec2(progressBarWidth, 0), overlay.empty() ? nullptr : overlay.c_str());
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
bool centerButton(float windowCenterX, const char* label, float padding, float height) {
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
    float spacing = GetStyle().ItemSpacing.x, padding = GetStyle().WindowPadding.x;
    float availableWidth = GetContentRegionAvail().x - maxTextSize - 2 * padding - 2 * spacing;
    float sliderWidth = availableWidth * 0.65f;
    float inputWidth = availableWidth * 0.35f;

    // Unique identifier from parameter text
    std::string sliderLabel = std::string("##slider_") + prefix + parameterText;
    std::string inputLabel = std::string("##input_") + prefix + parameterText;

    // [--Label--|------Slider------|--Input--]
    BeginGroup();

    Text(parameterText);
    SameLine();
    SetCursorPosX(maxTextSize + padding + spacing);

    PushItemWidth(sliderWidth);
    int discrete = floatToDiscreteInt(value, minVal, maxVal);
    if (SliderInt(sliderLabel.c_str(), &discrete, 1, 10)) {
        value = discreteIntToFloat(discrete, minVal, maxVal);
        onModified();
    }
    PopItemWidth();
    SameLine();

    PushItemWidth(inputWidth);
    if (InputFloat(inputLabel.c_str(), &value, step, stepFast, format, ImGuiInputTextFlags_CharsDecimal)) {
        value = std::clamp(value, minVal, maxVal);
        discrete = floatToDiscreteInt(value, minVal, maxVal);
        onModified();
    }
    PopItemWidth();

    EndGroup();
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


/*
Since we want to center text in a window, almost always do auto [windowCenterX, wrapWidth] = SetWindowPositionAndSize(...) for 
convenient text centering with subsequent calls to "centerWrappedText(...)"

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
pair<float, float> SetWindowPositionAndSize(int numRows, int numCols, int row, int col, int rowSize, int colSize) {
    row--; col--;
    ImGuiIO& io = GetIO();

    // Calculate the size of each segment
    float segmentWidth = io.DisplaySize.x / numCols;
    float segmentHeight = io.DisplaySize.y / numRows;

    // Calculate position based on the grid
    float xPos = segmentWidth * col;
    float yPos = segmentHeight * row;

    SetNextWindowPos(ImVec2(xPos, yPos), ImGuiCond_Always);
    SetNextWindowSize(ImVec2(colSize * segmentWidth, rowSize * segmentHeight), ImGuiCond_Always);

    // Return [centerX, wrapWidth] of the current window
    return make_pair(colSize * segmentWidth / 2, (colSize * segmentWidth) - (2 * GetStyle().WindowPadding.x));
}


void SeparatorSpaced(float space) {
    Dummy(ImVec2(0, space));
    Separator();
    Dummy(ImVec2(0, space));
}

/*
* Draw think horizontal line for separation
*/
void SeparatorSpacedThick(float space, float thickness, const ImVec4& color) {
    Dummy(ImVec2(0, space));
    Dummy(ImVec2(0.0f, thickness));
    ImVec2 pos = GetCursorScreenPos();
    GetWindowDrawList()->AddRectFilled(
        ImVec2(pos.x, pos.y - thickness),
        ImVec2(pos.x + GetContentRegionAvail().x, pos.y),
        ColorConvertFloat4ToU32(color)
    );
    Dummy(ImVec2(0, space));
}

