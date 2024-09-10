////////////////////////////////////////////////////////////////////////////////
// Callbacks.cpp -- Callback handlers -- rz -- 2024-08
// Copyright (c) 2024 Ricky Zhang
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "Callbacks.h"

/**
* Callback function to adjust the viewport when the window is resized.
*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));
    if (!data || (width == *(data->windowWidth) && height == *(data->windowHeight))) {
        return;
    }

    *(data->windowWidth) = width;
    *(data->windowHeight) = height;

    int newWidth = width;
    int newHeight = static_cast<int>(width / data->aspectRatio);
    if (newHeight > height) {
        newHeight = height;
        newWidth = static_cast<int>(height * data->aspectRatio);
    }
    if (newWidth != width || newHeight != height) {
        glfwSetWindowSize(window, newWidth, newHeight);
    }

    glViewport(0, 0, newWidth, newHeight);
    *(data->projection) = glm::perspective(glm::radians(45.0f), (float)newWidth / newHeight, 0.1f, 100.0f);
    data->shader->use();
    data->shader->setUniformMat4("projection", *(data->projection));
}

/**
* Keyboard callback.
*
* @param window                 [in] The parent window.
* 
* @param key                    [in] The GLFW key code such as the letter 'A'.
* 
* @param scancode               [in] The raw key number.  For example, the cursor
*                               keys do not correspond to letters.
* 
* @param action                 [in] The key action that occured (press, release,
*                               or release).
* 
*/

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));
    if (!data) return;

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_TAB:  // Toggle display of the "Info Screen".
                data->showInfoScreen = !data->showInfoScreen;
                break;
            case GLFW_KEY_D:
                if ((mods & GLFW_MOD_CONTROL)) {  // CTRL-D: toggle debug mode.
                    data->debugMode = !data->debugMode;
                    std::cout << "Debug mode is " << (data->debugMode ? "On" : "Off") << std::endl;
                    break;
                }  // Else fall into code below
            case GLFW_KEY_A:
            case GLFW_KEY_E:
            case GLFW_KEY_S:
            case GLFW_KEY_Q:
            case GLFW_KEY_W:
            case GLFW_KEY_ESCAPE:
                data->cameraState->camera->processKeyboard(key, data->deltaTime, data->boundCamera);
                break;
            case GLFW_KEY_UNKNOWN:  // We don't use this key.
                break;

        }
    }
}

/**
* Mouse button handler.
*/

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));
        if (!data) return;

        glm::vec3 ray_world = screenToWorldCoordinates(window, xpos, ypos, data->cameraState->camera->getViewMatrix(), *data->projection);

        // Assuming you have a function that checks intersection with objects in your scene
        std::string clickedObject = checkIntersection(ray_world);
        std::cout << "Left mouse button clicked! Ray: " << ray_world[0] << ", " << ray_world[1] << ", " << ray_world[2] << " Object: " << clickedObject << std::endl;
    }
}

/**
* Mouse movement callback.
*
* This adjust the viewport when the window is resized, and moves the
* camera when the right mouse button is pressed.
*/

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
    ImGuiIO& io = ImGui::GetIO();

    // Process camera movement only when the right mouse button is pressed and ImGui is not capturing the mouse
    if (!io.WantCaptureMouse || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));
        if (!data || !data->cameraState) return;
        CameraState* cameraState = data->cameraState;

        if (cameraState->firstMouse) {
            cameraState->lastX = xpos;
            cameraState->lastY = ypos;
            cameraState->firstMouse = false;
        }

        double xoffset = xpos - cameraState->lastX;
        double yoffset = cameraState->lastY - ypos;
        cameraState->lastX = xpos;
        cameraState->lastY = ypos;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            cameraState->camera->processMouseMovement((float)xoffset, (float)yoffset);
        }
        else {
            cameraState->firstMouse = true;
        }
    }
}

#if 0
/**
* Process keyboard input.
*
* This routine is called at the top of the rendering loop.  Note that glfwGetKey
* returns the "last reported state" of a particular key, which can be GLFW_PRESS
* or GLFW_RELEASE.
*
* @param window             [in] The window.
*
* @param deltaTime          [in] Elapsed time in glfw units. Used for velocity calculations.
*/

// Handle key presses
void processInput(GLFWwindow* window, float deltaTime) {
    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));
    if (!data || !data->cameraState) return;
    CameraState* cameraState = data->cameraState;

    bool boundCamera = data->boundCamera;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraState->camera->processKeyboard(GLFW_KEY_W, deltaTime, boundCamera);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraState->camera->processKeyboard(GLFW_KEY_S, deltaTime, boundCamera);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraState->camera->processKeyboard(GLFW_KEY_A, deltaTime, boundCamera);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraState->camera->processKeyboard(GLFW_KEY_D, deltaTime, boundCamera);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraState->camera->processKeyboard(GLFW_KEY_Q, deltaTime, boundCamera);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraState->camera->processKeyboard(GLFW_KEY_E, deltaTime, boundCamera);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        cameraState->camera->processKeyboard(GLFW_KEY_ESCAPE, deltaTime, boundCamera);
}
#endif

/**
* Scroll handler.
*
* This handles camera speed and zoom.
*/

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    auto* data = static_cast<GameControl*>(glfwGetWindowUserPointer(window));
    if (!data || !data->cameraState) return;
    CameraState* cameraState = data->cameraState;

    cameraState->camera->processMouseScroll((float)yoffset);
}
