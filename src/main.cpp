////////////////////////////////////////////////////////////////////////////////
// main.cpp -- Beyblade main program -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <string>
#include <vector>

#include "GameEngine.h"
#include "MeshCache.h"
#include "StateFactory.h"
#include "Utils.h"

int main() {
    GL_CHECK("begin");

    GameEngine* game = new GameEngine();

    if (!game->init("Battlebeyz", 1600, 900)) {
        return -1;
    }

    /* ----------------------OBJECT SETUP-------------------------- */

    // These might be null for now, quell errors
    //glm::vec3 initialPosition1 = glm::vec3(0.0f, 1.0f, 0.3f);
    //glm::vec3 initialPosition2 = glm::vec3(0.0f, 1.0f, -0.3f);
    //glm::vec3 initialVelocity1 = glm::vec3(0.0f, 0.0f, -0.1f);
    //glm::vec3 initialVelocity2 = glm::vec3(0.0f, 0.0f, 0.1f);
    //glm::vec3 initialAngularVelocity = glm::vec3(0.0f, -450.0f, 0.0f);
    //beyblade1->getBody()->setInitialLaunch(initialPosition1, initialVelocity1, initialAngularVelocity);
    //beyblade2->getBody()->setInitialLaunch(initialPosition2, initialVelocity2, initialAngularVelocity);

    /* ----------------------MAIN RENDERING LOOP-------------------------- */

    // Parsing every model the save file references is the slow part of startup, and the
    // models are independent, so the loading screen fans them across the job system.
    std::vector<std::function<bool()>> loadTasks;
    for (const std::string& modelPath : game->collectSaveModelPaths()) {
        loadTasks.push_back([modelPath]() { return MeshCache::getInstance().prewarm(modelPath); });
    }

    game->pushState(StateFactory::createLoadingState(
        game,
        loadTasks,
        [game]() {
            // Meshes are cached now, so building the profiles is just JSON plus uploads
            game->attemptSaveDataLoad();

            game->changeState(StateFactory::createState(game, GameStateType::HOME));
        }
    ));
    game->applyPendingTransitions();

    while (game->running()) {
        game->update();        // Time-based state updates
        if (!game->paused) game->handleEvents();  // External inputs: user/system
        game->draw();          // Render the current state
        game->applyPendingTransitions();  // Swap states only at the frame boundary
    }
    delete game;

    return 0;
}