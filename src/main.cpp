////////////////////////////////////////////////////////////////////////////////
// main.cpp -- Beyblade main program -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#include "PhysicsWorld.h"
#include "GameEngine.h"
#include "ProfileManager.h"
#include "StateFactory.h"

#include "Utils.h"

int main() {
    GL_CHECK("begin");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    GameEngine* game = new GameEngine();

    if (!game->init("Battlebeyz", 1600, 900)) {
        return -1;
    }

    // Declare physics world with default parameters
    auto physicsWorld = new PhysicsWorld();

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

    //std::function<bool()> wait = { [&]() -> bool { MockLoading::getInstance().mock(); return true; } };
    //game->pushState(StateFactory::createLoadingState(
    //    game,
    //    {wait, wait, wait},
    //    [&]() {
    //        game->changeState(StateFactory::createState(game, GameStateType::HOME));
    //    }
    //));
    game->changeState(StateFactory::createState(game, GameStateType::HOME));

    while (game->running()) {
        game->update();        // Time-based state updates
        if (!game->paused) game->handleEvents();  // External inputs: user/system
        game->draw();          // Render the current state
    }
    delete game;

    return 0;
}