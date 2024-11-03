#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>
#include "InputUtils.h"

class InputManager {
public:
    /* GENERAL */
    void setWindow(GLFWwindow* window) { this->window = window; }
    void updateState() {
        prevKeyStates = keyStates;
        prevMouseButtonStates = mouseButtonStates;
    }

    /* KEYS */
    void setKey(int key, bool isPressed) { keyStates[key] = isPressed; }
    bool keyPressed(int key) const {
        return keyStates.count(key) && keyStates.at(key);
    }
    bool keyJustPressed(int key) const {
        return keyStates.count(key) && keyStates.at(key) && (!prevKeyStates.count(key) || !prevKeyStates.at(key));
    }
    bool keyJustReleased(int key) const {
        return prevKeyStates.count(key) && prevKeyStates.at(key) && (!keyStates.count(key) || !keyStates.at(key));
    }
    bool keyJustPressed(const KeyCombination& combo) const {
        return keyJustPressed(combo.primaryKey) && keyJustPressed(combo.modifierKey);
    }

    /* MOUSE */
    void setMouseButton(int button, bool isPressed) { mouseButtonStates[button] = isPressed; }
    bool mouseButtonPressed(int button) const {
        return mouseButtonStates.count(button) && mouseButtonStates.at(button);
    }
    bool mouseButtonJustPressed(int button) const {
        return mouseButtonStates.count(button) && mouseButtonStates.at(button) && (!prevMouseButtonStates.count(button) || !prevMouseButtonStates.at(button));
    }
    void setMousePosition(double xpos, double ypos) {
        if (mouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            if (firstMouseMove) { // Initialize the mouse position on the first click
                lastX = xpos;
                lastY = ypos;
                firstMouseMove = false;
            }
            xOffset = xpos - lastX;
            yOffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;
        }
        else {
            resetMouseState();
        }
    }
    std::pair<float, float> getMouseOffsets() {
        auto offsets = std::make_pair(static_cast<float>(xOffset), static_cast<float>(yOffset));
        xOffset = 0.0;
        yOffset = 0.0;
        return offsets;
    }
    void resetMouseState() {
        xOffset = 0.0;
        yOffset = 0.0;
        firstMouseMove = true;
    }

    /* SCROLL */
    void setScrollOffset(double xoffset, double yoffset) {
        scrollOffsetY = static_cast<float>(yoffset);
        scrollMovedFlag = (scrollOffsetY != 0.0f);
    }
    bool scrollMoved() const {
        return scrollMovedFlag;
    }
    float getScrollOffsetY() const { return scrollOffsetY; }
    void resetScrollOffset() {
        scrollOffsetY = 0.0f;
        scrollMovedFlag = false;
    }

private:
    GLFWwindow* window = nullptr;
    std::unordered_map<int, bool> keyStates;
    std::unordered_map<int, bool> prevKeyStates;

    std::unordered_map<int, bool> mouseButtonStates;
    std::unordered_map<int, bool> prevMouseButtonStates;

    bool firstMouseMove = true;
    double lastX = 0.0, lastY = 0.0;
    double xOffset = 0.0, yOffset = 0.0;

    float scrollOffsetY = 0.0f;
    bool scrollMovedFlag = false;
};
