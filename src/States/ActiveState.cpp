#include "ActiveState.h"
#include "../GameEngine.h"
#include "StateIdentifiers.h"

void ActiveState::init()
{
    // Set up keyboard and mouse callbacks.

    GLFWwindow* window = game->getWindow();

    glfwSetKeyCallback(window, ActiveState::keyCallback);
    glfwSetMouseButtonCallback(window, ActiveState::mouseButtonCallback);
    glfwSetCursorPosCallback(window, ActiveState::mouseCallback);
    glfwSetScrollCallback(window, ActiveState::scrollCallback);
}

void ActiveState::cleanup()
{
    // Remove keyboard and mouse callbacks.

    GLFWwindow* window = game->getWindow();

    glfwSetKeyCallback(window, nullptr);
    glfwSetMouseButtonCallback(window, nullptr);
    glfwSetCursorPosCallback(window, nullptr);
    glfwSetScrollCallback(window, nullptr);
}

void ActiveState::pause() {}

void ActiveState::resume() {}

void ActiveState::handleEvents() {}

void ActiveState::update(float deltaTime) {}

void ActiveState::draw() {
    ImGui::Begin("About");
    if (ImGui::Button("Back to Home")) {
        game->changeState(GameStateType::HOME);
    }
    ImGui::Text("This is the Active screen.");
    ImGui::End();
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
*/

// Track if ESC was pressed
static bool escPressed = false;

/* static */
void ActiveState::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    auto* game = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
    if (!game) return;

    auto gs = game->getGameState();
    if (gs && gs->getStateType() == GameStateType::ACTIVE) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            switch (key) {
            case GLFW_KEY_TAB:  // Toggle display of the "Info Screen".
                //?? TODO ?? engine->showInfoScreen = !engine->showInfoScreen;
                break;
            case GLFW_KEY_D:
                if ((mods & GLFW_MOD_CONTROL)) {  // CTRL-D: toggle debug mode.
                    game->debugMode = !game->debugMode;
                    std::cout << "Debug mode is " << (game->debugMode ? "On" : "Off") << std::endl;
                    break;
                }  // Else fall into code below
            case GLFW_KEY_A:
            case GLFW_KEY_E:
            case GLFW_KEY_S:
            case GLFW_KEY_Q:
            case GLFW_KEY_W:
                game->cameraState->camera->processKeyboard(key, game->deltaTime, game->boundCamera);
                break;
            case GLFW_KEY_ESCAPE:
                // Toggle Options screen when ESC is pressed, but ensure it only happens on one press
                if (!escPressed) {
                    //?? TODO ?? engine->showOptionsScreen = !engine->showOptionsScreen;
                    escPressed = true;
                }
                break;
            case GLFW_KEY_UNKNOWN:  // We don't use this key.
                break;
            }
        }
    }

    // Handle key release to reset the escPressed flag
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_ESCAPE) {
            escPressed = false;
        }
    }
}

/**
* Mouse button handler.
*/

/* static */
void ActiveState::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // TODO: Refactor, as GameControl is no longer valid
        //auto* game = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
        //if (!game) return;

        //glm::vec3 ray_world = screenToWorldCoordinates(window, xpos, ypos, data->cameraState->camera->getViewMatrix(), *data->projection);

        //// Assuming you have a function that checks intersection with objects in your scene
        //std::string clickedObject = checkIntersection(ray_world);
        //std::cout << "Left mouse button clicked! Ray: " << ray_world[0] << ", " << ray_world[1] << ", " << ray_world[2] << " Object: " << clickedObject << std::endl;
    }
}

/**
* Mouse movement callback.
*
* This adjust the viewport when the window is resized, and moves the
* camera when the right mouse button is pressed.
*/

/* static */
void ActiveState::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
    ImGuiIO& io = ImGui::GetIO();

    // Process camera movement only when the right mouse button is pressed and ImGui is not capturing the mouse
    if (!io.WantCaptureMouse || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        auto* game = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
        if (!game || !game->cameraState) return;
        CameraState* cameraState = game->cameraState;

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

/**
* Scroll handler.
*
* This handles camera speed and zoom.
*/

/* static */
void ActiveState::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    auto* game = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
    if (!game || !game->cameraState) return;
    CameraState* cameraState = game->cameraState;

    cameraState->camera->processMouseScroll((float)yoffset);
}
