#pragma once
#include <unordered_map>
#include <vector>
#include <initializer_list>
#include <GLFW/glfw3.h>

enum Action {
    MoveForward, MoveBackward, MoveLeft, MoveRight, MoveUp, MoveDown
};
extern std::unordered_map<int, Action> movementKeys;

struct KeyCombination {
    int primaryKey;
    int modifierKey;

    KeyCombination(int primary, int modifier)
        : primaryKey(primary), modifierKey(modifier) {}
};

// Example: Ctrl + D combination
const KeyCombination CtrlD(GLFW_KEY_D, GLFW_KEY_LEFT_CONTROL);
