#include "InputManager.h"

void InputManager::setKey(int key, bool isPressed) {
    keyStates[key] = isPressed;
}

bool InputManager::isKeyPressed(int key) const {
    auto it = keyStates.find(key);
    return it != keyStates.end() && it->second;
}

void InputManager::setMouseButton(int button, bool isPressed) {
    mouseButtonStates[button] = isPressed;
}

bool InputManager::isMouseButtonPressed(int button) const {
    auto it = mouseButtonStates.find(button);
    return it != mouseButtonStates.end() && it->second;
}

void InputManager::setMousePosition(double xPos, double yPos) {
    mousePosition = glm::vec2(static_cast<float>(xPos), static_cast<float>(yPos));
}

glm::vec2 InputManager::getMousePosition() const {
    return mousePosition;
}

void InputManager::setScrollOffset(double xOffset, double yOffset) {
    scrollOffset = glm::vec2(static_cast<float>(xOffset), static_cast<float>(yOffset));
}

glm::vec2 InputManager::getScrollOffset() const {
    return scrollOffset;
}

void InputManager::resetScrollOffset() {
    scrollOffset = glm::vec2(0.0f);
}
