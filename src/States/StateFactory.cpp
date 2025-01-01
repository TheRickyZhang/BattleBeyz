#include "StateFactory.h"
#include "StateIdentifiers.h"

#include "HomeState.h"
#include "ActiveState.h"
#include "CustomizeState.h"
#include "AboutState.h"
#include "PauseState.h"
#include "LoadingState.h"

// Implement the createState function that returns a unique_ptr to the correct GameState based on GameStateType
std::unique_ptr<GameState> StateFactory::createState(GameEngine* game, GameStateType stateType) {
    switch (stateType) {
    case GameStateType::HOME:
        return std::make_unique<HomeState>(game); // Create and return a HomeState
    case GameStateType::ACTIVE:
        return std::make_unique<ActiveState>(game); // Create and return an ActiveState
    case GameStateType::CUSTOMIZE:
        return std::make_unique<CustomizeState>(game); // Create and return a CustomizeState
    case GameStateType::ABOUT:
        return std::make_unique<AboutState>(game); // Create and return an AboutState
    case GameStateType::PAUSE:
        return std::make_unique<PauseState>(game); // Create and return a PauseState
    case GameStateType::LOADING:
        return std::make_unique<LoadingState>(game); // Loading state is not implemented in this example
    default:
        return nullptr; // Return nullptr if the state type is unknown
    }
}
