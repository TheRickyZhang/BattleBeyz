////////////////////////////////////////////////////////////////////////////////
// UI.h -- User Interface include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <cmath>
#include <vector>
#include <string>

// NOTE: This ordering is necessary to not have a glew inclusion error
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GameControl.h"
#include "Texture.h"
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <functional>

const ImGuiWindowFlags MinimalWindow = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize; // No decorations, navigation, moving, or resizing
const ImGuiWindowFlags OverlayWindow = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize; // No decorations, background, inputs; auto-resizes
const ImGuiWindowFlags StaticWindow = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse; // Fixed position/size, no decorations, cannot move/resize/collapse
const ImGuiWindowFlags ScrollingWindow = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar; // No decorations, no collapse, horizontal scrollbar
const ImGuiWindowFlags NormalWindow = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse; // Auto-resizes, cannot collapse

// Handy general ImGui rendering functions
void centerWrappedText(float windowCenterX, float wrapWidth, const char* text);
void textWithLink(const char* text, const char* url);
void renderPlainScreen(const char* windowID, float windowCenterX, float windowCenterY, float width, float height);

ImVec2 calculateButtonSize(const char* text, float padding, float height);
bool centerButton(float windowCenterX, const char* text, float padding, float height);
bool renderExitButton(GLFWwindow* window, const ImVec4& buttonColor, const ImVec4& buttonHovered, const ImVec4& buttonActive);

void showTooltip(const char* text);
bool sliderWithReset(const char* label, float* value, float min, float max, float defaultValue);
bool colorPickerWithReset(const char* label, ImVec4* color, const ImVec4& defaultColor);
bool collapsingSectionWithToggle(const char* label, bool* isVisible);
void centerColoredText(float windowCenterX, const ImVec4& color, const char* text);
void DrawDiscreteFloatControl(const char* parameterText, float maxTextSize, const char* prefix, float& value, float minVal, float maxVal, float step, float stepFast, const char* format, std::function<void()> onModified);

// Gets the maximum width of a list of strings (ensure all buttons are the same size)
float getMaxWidth(const std::vector<std::string>& text);

//Segments the screen nicely with fractions and 1-based indexing
void SetWindowPositionAndSize(int numRows, int numCols, int row, int col, int rowSize = 1, int colSize = 1);


// TODO: Outdated? 
void showAboutScreen(GLFWwindow* window, Texture& backgroundTexture);
void showCustomizeScreen(GLFWwindow* window, Texture& backgroundTexture);
void showHomeScreen(GLFWwindow* window, Texture& homeScreenTexture, Texture& backgroundTexture);
void drawInfoScreen(GLFWwindow* window);
void showLoadingScreen(GLFWwindow* window, Texture& backgroundTexture, const char* message = "Loading...");
void showOptionsScreen(GLFWwindow* window);


static void UICustomScreenProfileSelected(int selectedItemIndex);
static void UICustomScreenInit(GameControl* gc);
static void UIProfileCreateSectionFromDefaults(const char* profileName, Disc& disc, Driver& driver, Layer& layer);
static void UIPromptForProfile();
//static void UIProfileCreateSectionFromScreen(const std::string& profileName);
//static void UICustomSettingsSave(GameControl* gc);
static void UIUpdateBladeFromProfile();

void UISetRunState(bool isError, const std::string& msg);

