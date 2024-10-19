////////////////////////////////////////////////////////////////////////////////
// Callbacks.h -- Callbacks include -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Camera.h"
#include "GameControl.h"
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "Utils.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Define callbacks to be used in main
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);