#include "InputUtils.h"

std::unordered_map<int, Action> movementKeys = {
    {GLFW_KEY_W, Action::MoveForward},
    {GLFW_KEY_S, Action::MoveBackward},
    {GLFW_KEY_A, Action::MoveLeft},
    {GLFW_KEY_D, Action::MoveRight},
    {GLFW_KEY_Q, Action::MoveDown},
    {GLFW_KEY_E, Action::MoveUp}
};