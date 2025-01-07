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

    Beyblade* beyblade1 = game->pm.getActiveProfile()->getBeyblade(1).get();
    Beyblade* beyblade2 = game->pm.getActiveProfile()->getBeyblade(2).get();
    // Add beys
    physicsWorld->addBeyblade(beyblade1);
    physicsWorld->addBeyblade(beyblade2);
    glm::vec3 initialPosition1 = glm::vec3(0.0f, 1.0f, 0.3f);
    glm::vec3 initialPosition2 = glm::vec3(0.0f, 1.0f, -0.3f);
    glm::vec3 initialVelocity1 = glm::vec3(0.0f, 0.0f, -0.1f);
    glm::vec3 initialVelocity2 = glm::vec3(0.0f, 0.0f, 0.1f);
    glm::vec3 initialAngularVelocity = glm::vec3(0.0f, -450.0f, 0.0f);
    beyblade1->getRigidBody()->setInitialLaunch(initialPosition1, initialVelocity1, initialAngularVelocity);
    beyblade2->getRigidBody()->setInitialLaunch(initialPosition2, initialVelocity2, initialAngularVelocity);

    /* ----------------------MAIN RENDERING LOOP-------------------------- */

    std::function<bool()> wait = { [&]() -> bool { MockLoading::getInstance().mock(); return true; } };
    game->pushState(StateFactory::createLoadingState(
        game,
        {wait, wait, wait},
        [&]() {
            game->changeState(GameStateType::HOME);
        }
    ));

    while (game->running()) {
#if 1
        auto currentTime = static_cast<float>(glfwGetTime());
        game->deltaTime = currentTime - game->prevTime;
        game->prevTime = currentTime;
# else
        game->deltaTime = 0.0052f; // fixed frame rate, lower = slower
#endif

        if (!game->paused) game->handleEvents();  // External inputs: user/system
        game->update();        // Time-based state updates
        game->draw();          // Render the current state
    }
    return 0;
}