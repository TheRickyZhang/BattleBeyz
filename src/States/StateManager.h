#pragma once

#include <memory>
#include "GameState.h"

class StateManager {
public:
    StateManager() : currentState(nullptr) {}

    void ChangeState(std::unique_ptr<GameState> newState) {
        if (currentState) {
            currentState->cleanup(); // Previously OnExit
        }
        currentState = std::move(newState);
        if (currentState) {
            currentState->init(); // Previously OnEnter
        }
    }

    void HandleInput(GLFWwindow* window) {
        if (currentState)
            currentState->handleEvents(nullptr); // Call handleEvents with game or other parameters as needed
    }

    void Update(float deltaTime) {
        if (currentState)
            currentState->update(nullptr, deltaTime); // Call update with game or other parameters as needed
    }

    void Render() {
        if (currentState)
            currentState->draw(nullptr); // Call draw with game or other parameters as needed
    }

private:
    std::unique_ptr<GameState> currentState;
};
