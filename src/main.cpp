////////////////////////////////////////////////////////////////////////////////
// main.cpp -- Beyblade main program -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

// Among other things, this includes <windows.h>, imgui_xxx, and GLFW/glfw3.h
#include "UI.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ShaderProgram.h"
#include "TextRenderer.h"
#include "ShaderPath.h"

#include "Buffers.h"
#include "Stadium.h"
#include "Camera.h"
#include "QuadRenderer.h"
#include "Physics.h"

#include "PhysicsWorld.h"
#include "RigidBody.h"
#include "Beyblade.h"
#include "RigidBodies/BeybladeParts.h"
#include "RigidBodies/BeybladeBody.h"
#include "RigidBodies/StadiumBody.h"
#include "MeshObjects/BeybladeMesh.h"
#include "MeshObjects/StadiumMesh.h"

#include "GameEngine.h"
#include "ProfileManager.h"

#include <iomanip>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <atomic>

int main() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    GameEngine engine;

    if (!engine.init("Battlebeyz", 1600, 900)) {
        return -1;
    }

    // Declare physics world with default parameters
    auto physicsWorld = new PhysicsWorld();

    /* ----------------------OBJECT SETUP-------------------------- */

    // These might be null for now, quell errors

    Beyblade* beyblade1 = engine.pm.getActiveProfile()->getBeyblade(1).get();
    Beyblade* beyblade2 = engine.pm.getActiveProfile()->getBeyblade(2).get();

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

    // TODO: Move these declarations dynmically upon loading of ActiveState
    engine.pushState(GameStateType::LOADING);

    while (engine.running()) {
#if 1
        auto currentTime = static_cast<float>(glfwGetTime());
        engine.deltaTime = currentTime - engine.prevTime;
        engine.prevTime = currentTime;
# else
        engine.deltaTime = 0.0052f; // fixed frame rate, lower = slower
#endif

        if (!engine.paused) engine.handleEvents();  // External inputs: user/system
        engine.update();        // Time-based state updates
        engine.draw();          // Render the current state
    }
    return 0;
}