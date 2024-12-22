#include "InputManager.h"

InputManager& InputManager::getInstance() {
    static InputManager instance;
    return instance;
}

void InputManager::setWindow(GLFWwindow* window) {
    this->window = window;
}
void InputManager::updateState() {
    prevKeyStates = keyStates;
    prevMouseButtonStates = mouseButtonStates;
}

void InputManager::setKey(int key, bool isPressed) {
    keyStates[key] = isPressed;
}
bool InputManager::keyPressed(int key) const {
    return keyStates.count(key) && keyStates.at(key);
}
bool InputManager::keyJustPressed(int key) const {
    return keyStates.count(key) && keyStates.at(key) &&
        (!prevKeyStates.count(key) || !prevKeyStates.at(key));
}
bool InputManager::keyJustReleased(int key) const {
    return prevKeyStates.count(key) && prevKeyStates.at(key) &&
        (!keyStates.count(key) || !keyStates.at(key));
}
bool InputManager::keyJustPressed(const KeyCombination& combo) const {
    return keyJustPressed(combo.primaryKey) && keyJustPressed(combo.modifierKey);
}

void InputManager::setMouseButton(int button, bool isPressed) {
    mouseButtonStates[button] = isPressed;
}
bool InputManager::mouseButtonPressed(int button) const {
    return mouseButtonStates.count(button) && mouseButtonStates.at(button);
}
bool InputManager::mouseButtonJustPressed(int button) const {
    return mouseButtonStates.count(button) && mouseButtonStates.at(button) &&
        (!prevMouseButtonStates.count(button) || !prevMouseButtonStates.at(button));
}
void InputManager::setMousePosition(double xpos, double ypos) {
    if (mouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        if (firstMouseMove) {
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
std::pair<float, float> InputManager::getMouseOffsets() {
    auto offsets = std::make_pair(static_cast<float>(xOffset), static_cast<float>(yOffset));
    xOffset = 0.0;
    yOffset = 0.0;
    return offsets;
}
void InputManager::resetMouseState() {
    xOffset = 0.0;
    yOffset = 0.0;
    firstMouseMove = true;
}

void InputManager::setScrollOffset(double xoffset, double yoffset) {
    scrollOffsetY = static_cast<float>(yoffset);
    scrollMovedFlag = (scrollOffsetY != 0.0f);
}
bool InputManager::scrollMoved() const {
    return scrollMovedFlag;
}
float InputManager::getScrollOffsetY() const {
    return scrollOffsetY;
}
void InputManager::resetScrollOffset() {
    scrollOffsetY = 0.0f;
    scrollMovedFlag = false;
}
