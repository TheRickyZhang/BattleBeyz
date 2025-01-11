#pragma once

#include <unordered_map>

#include "InputUtils.h"

/*
* Singleton. Handles all conversion of inputs to actions. Should it take a window pointer?
* 
* Currently does not support key combinations.
*/
class InputManager {
public:
    /* Singleton */
    static InputManager& getInstance();

    /* General */
    void updateState();

    /* Keys */
    void setKey(int key, bool isPressed);
    bool keyPressed(int key) const;
    bool keyJustPressed(int key) const;
    bool keyJustReleased(int key) const;
    bool keyJustPressed(const KeyCombination& combo) const;

    /* Mouse */
    void setMouseButton(int button, bool isPressed);
    bool mouseButtonPressed(int button) const;
    bool mouseButtonJustPressed(int button) const;
    void setMousePosition(double xpos, double ypos);
    std::pair<float, float> getMouseOffsets();
    void resetMouseState();

    /* Scroll */
    void setScrollOffset(double xoffset, double yoffset);
    bool scrollMoved() const;
    float getScrollOffsetY() const;
    void resetScrollOffset();

private:
    InputManager() = default;
    ~InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

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
