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

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GameControl.h"
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include "Texture.h"

void showHomeScreen(GLFWwindow* window, Texture& homeScreenTexture, Texture& backgroundTexture);
void showInfoScreen(GLFWwindow* window, float (*imguiColor)[3]);
void showCustomizeScreen(GLFWwindow* window, Texture& backgroundTexture);
void showAboutScreen(GLFWwindow* window, Texture& backgroundTexture);
void showLoadingScreen(GLFWwindow* window, Texture& backgroundTexture, const char* message = "Loading...");