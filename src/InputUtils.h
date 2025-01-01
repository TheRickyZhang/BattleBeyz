#pragma once
#include <unordered_map>
#include <vector>

#include <GLFW/glfw3.h>

enum Action {
    MoveForward, MoveBackward, MoveLeft, MoveRight, MoveUp, MoveDown
};
extern std::unordered_map<int, Action> movementKeys; // TOLOOK: use of extern

struct KeyCombination {
    int primaryKey;
    int modifierKey;

    KeyCombination(int primary, int modifier)
        : primaryKey(primary), modifierKey(modifier) {}
};

// Example: Ctrl + D combination
const KeyCombination CtrlD(GLFW_KEY_D, GLFW_KEY_LEFT_CONTROL);
