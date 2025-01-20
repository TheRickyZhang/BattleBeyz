////////////////////////////////////////////////////////////////////////////////
// UI.h -- User Interface include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include <stb_image.h>
#include <functional>
#include <utility>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

class FrameBufferRenderer;

 // Nothing, no interaction
const ImGuiWindowFlags MinimalWindow = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

// Expands to text size
const ImGuiWindowFlags AutoResizeWindow = ImGuiWindowFlags_AlwaysAutoResize;


// Currently unused
bool colorPickerWithReset(const char* label, ImVec4* color, const ImVec4& defaultColor);

// Centering Text
void centerWrappedText(float centerX, float wrapWidth, const std::string& text);
void centerProgressBar(float centerX, float barWidth, float progress, const std::string& overlay = "");

void textWithLink(const char* text, const char* url);

// Render Buttons
ImVec2 calculateButtonSize(const char* text, float padding, float height);
bool centerButton(float windowCenterX, const char* text, float padding = 20.0f, float height = 30.0f);
bool renderExitButton(GLFWwindow* window, const ImVec4& buttonColor, const ImVec4& buttonHovered, const ImVec4& buttonActive);

void showTooltip(const char* text);
bool collapsingSectionWithToggle(const char* label, bool* isVisible);
void centerColoredText(float windowCenterX, const ImVec4& color, const char* text);

// Separators
void SeparatorSpaced(float space = 10.0f);
void SeparatorSpacedThick(float space = 10.0f, float thickness = 5.0f, const ImVec4& color = ImGui::GetStyleColorVec4(ImGuiCol_Separator));

// Sliders
bool SliderIntDiscrete(const char* label, int* value, int minVal, int maxVal, int step);

// Bulk Utility for specialized drawing
float getMaxWidth(const std::vector<std::string>& text);                        // Returns the max width of any singular string for alignment
void DrawDiscreteFloatControl(const char* parameterText, float maxTextSize,     // Render a connected slider/input for convenient editing of stats
    const char* prefix, float& value, float minVal, float maxVal, float step,
    float stepFast, const char* format, std::function<void()> onModified);

//Segments the screen nicely with fractions and 1-based indexing
std::pair<float, float> SetWindowPositionAndSize(int numRows, int numCols, int row, int col, int rowSize = 1, int colSize = 1);
