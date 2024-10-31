#pragma once
#include <unordered_map>
#include <glm/vec2.hpp>

// Very distant to-do: expand class to allow for more complex handling of input, such as combinations, different controllers, and key remappings
class InputManager {
public:
    // Keyboard
    void setKey(int key, bool isPressed);
    bool isKeyPressed(int key) const;

    // Mouse button/position
    void setMouseButton(int button, bool isPressed);
    bool isMouseButtonPressed(int button) const;
    void setMousePosition(double xPos, double yPos);
    glm::vec2 getMousePosition() const;

    // Mouse scroll
    void setScrollOffset(double xOffset, double yOffset);
    glm::vec2 getScrollOffset() const;
    void resetScrollOffset(); // Call this after processing scroll input (?)

private:
    // Key: GLFW key code, Value: true if pressed
    std::unordered_map<int, bool> keyStates;
    
    // Mouse button states
    std::unordered_map<int, bool> mouseButtonStates;

    // Mouse cursor position
    glm::vec2 mousePosition = glm::vec2(0.0f);

    // Scroll offset since last frame
    glm::vec2 scrollOffset = glm::vec2(0.0f);
};
