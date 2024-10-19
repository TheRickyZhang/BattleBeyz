////////////////////////////////////////////////////////////////////////////////
// UI.h -- User Interface include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GameControl.h"
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include "Texture.h"

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

// Gets the maximum width of a list of strings (ensure all buttons are the same size)
float getMaxWidth(const std::vector<std::string>& text);

void showAboutScreen(GLFWwindow* window, Texture& backgroundTexture);
void showCustomizeScreen(GLFWwindow* window, Texture& backgroundTexture);
void showHomeScreen(GLFWwindow* window, Texture& homeScreenTexture, Texture& backgroundTexture);
void showInfoScreen(GLFWwindow* window, float (*imguiColor)[3]);
void showLoadingScreen(GLFWwindow* window, Texture& backgroundTexture, const char* message = "Loading...");
void showOptionsScreen(GLFWwindow* window);

static void UICustomScreenProfileSelected(int selectedItemIndex);
static void UICustomScreenInit(GameControl* gc);
static void UIProfileCreateSectionFromDefaults(const char* profileName, Disc& disc, Driver& driver, Layer& layer);
static void UIPromptForProfile();
static void UIProfileCreateSectionFromScreen(const std::string& profileName);
static void UICustomSettingsSave(GameControl* gc);
static void UIUpdateBladeFromProfile();

void UISetRunState(bool isError, const std::string& msg);

