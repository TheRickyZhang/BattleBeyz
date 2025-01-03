////////////////////////////////////////////////////////////////////////////////
// UI.h -- User Interface include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

// NOTE: This ordering is necessary to not have a glew inclusion error
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Texture.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include <stb_image.h>
#include <functional>
#include <utility>

const ImGuiWindowFlags MinimalWindow = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize; // No decorations, navigation, moving, or resizing
const ImGuiWindowFlags OverlayWindow = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize; // No decorations, background, inputs; auto-resizes
const ImGuiWindowFlags StaticWindow = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse; // Fixed position/size, no decorations, cannot move/resize/collapse
const ImGuiWindowFlags ScrollingWindow = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar; // No decorations, no collapse, horizontal scrollbar
const ImGuiWindowFlags NormalWindow = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse; // Auto-resizes, cannot collapse

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

// Bulk Utility for specialized drawing
float getMaxWidth(const std::vector<std::string>& text);                        // Returns the max width of any singular string for alignment
void DrawDiscreteFloatControl(const char* parameterText, float maxTextSize,     // Render a connected slider/input for convenient editing of stats
    const char* prefix, float& value, float minVal, float maxVal, float step,
    float stepFast, const char* format, std::function<void()> onModified);

//Segments the screen nicely with fractions and 1-based indexing
std::pair<float, float> SetWindowPositionAndSize(int numRows, int numCols, int row, int col, int rowSize = 1, int colSize = 1);

