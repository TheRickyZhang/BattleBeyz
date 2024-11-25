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

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ShaderProgram.h"
#include "TextRenderer.h"
#include "Texture.h"
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

    auto stadiumTexture = new Texture("./assets/textures/Hexagon.jpg", "texture1");
    
    /* ----------------------OBJECT SETUP-------------------------- */

    //setupBuffers(tetrahedronVAO, tetrahedronVBO, tetrahedronEBO, tetrahedronVertices,
    //    sizeof(tetrahedronVertices), tetrahedronIndices, sizeof(tetrahedronIndices));

    // Initialize VAO, VBO, and EBO for the floor
    GLuint floorVAO, floorVBO, floorEBO;
    float floorVertices[] = {
        // Positions        // Normals       // TexCoords // Colors
        -30.0f, 0.0f, -30.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
        30.0f, 0.0f, -30.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f, 0.5f, 0.5f, 0.5f,
        30.0f, 0.0f,  30.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f, 0.5f, 0.5f, 0.5f,
        -30.0f, 0.0f,  30.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f, 0.5f, 0.5f, 0.5f,
    };

    unsigned int floorIndices[] = {
            0, 1, 2,
            2, 3, 0
    };
    setupBuffers(floorVAO, floorVBO, floorEBO, floorVertices, sizeof(floorVertices), floorIndices, sizeof(floorIndices));

    // Initialize VAO, VBO, and EBO for the floor
    GLuint stadiumVAO = 0, stadiumVBO = 0, stadiumEBO = 0;

    // Create the Stadium object

    // TODO: Why does this stadiumPosition not affect where the actual stadium rendered? (At default 0, 0, 0)
    glm::vec3 stadiumPosition = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 stadiumColor = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 ringColor = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 crossColor = glm::vec3(0.0f, 0.0f, 1.0f);
    float stadiumRadius = 1.2f;
    float stadiumCurvature = 0.10f;
    float stadiumCoefficientOfFriction = 0.35f;
    int numRings = 10;
    int sectionsPerRing = 64;
    float stadiumTextureScale = 1.5f;

    stadiumTexture->use();
    StadiumBody* rigidBody = new StadiumBody(stadiumPosition, stadiumRadius, stadiumCurvature, stadiumCoefficientOfFriction);
    StadiumMesh* stadiumMesh = new StadiumMesh(stadiumTexture, sectionsPerRing, numRings, ringColor, crossColor, stadiumColor, stadiumTextureScale);

    Stadium* stadium = new Stadium(rigidBody, stadiumMesh, "Stadium 1");
    physicsWorld->addStadium(stadium);

    // NEWMESH: Load meshes before bodies so we can get the actual object sizes. Then pass the
    // mesh to the BeybladdeBody constructor.
    // Also call mesh->initializeMesh() here.

    // These might be null for now, quell errors

    Beyblade* beyblade1 = engine.pm.getActiveProfile()->getBeyblade(1).get();
    Beyblade* beyblade2 = engine.pm.getActiveProfile()->getBeyblade(2).get();

    // Add beys
    physicsWorld->addBeyblade(beyblade1);
    physicsWorld->addBeyblade(beyblade2);
    glm::vec3 initialPosition1 = glm::vec3(0.0f, 2.0f, 0.5f);
    glm::vec initialPosition2 = glm::vec3(0.0f, 2.0f, -0.5f);
    glm::vec3 initialVelocity1 = glm::vec3(0.0f, 0.0f, -0.2f);
    glm::vec3 initialVelocity2 = glm::vec3(0.2f, 0.0f, 0.1f);
    glm::vec3 initialAngularVelocity = glm::vec3(0.0f, -450.0f, 0.0f);

    beyblade1->getRigidBody()->setInitialLaunch(initialPosition1, initialVelocity1, initialAngularVelocity);
    beyblade2->getRigidBody()->setInitialLaunch(initialPosition2, initialVelocity2, initialAngularVelocity);

    /* ----------------------MAIN RENDERING LOOP-------------------------- */

    // TODO: Move these declarations dynmically upon loading of ActiveState
    engine.camera->setFollowingBey(beyblade1->getRigidBody());
    engine.camera->setPanningVariables(stadium->getRigidBody());
    engine.pushState(GameStateType::LOADING);

    while (engine.running()) {
#if 1
        auto currentTime = static_cast<float>(glfwGetTime());
        engine.deltaTime = currentTime - engine.prevTime;
        engine.prevTime = currentTime;
# else
        engine.deltaTime = 0.0052f; // fixed frame rate, lower = slower
#endif

        engine.handleEvents();  // External inputs: user/system
        engine.update();        // Time-based state updates
        engine.draw();          // Render the current state
    }
    return 0;

//    while (!glfwWindowShouldClose(window)) {
//        // Measure delta time for physics
//        auto currentTime = static_cast<float>(glfwGetTime());
//#if 1
//        gameControl.deltaTime = currentTime - prevTime;
//        prevTime = currentTime;
//#else
//        gameControl.deltaTime = 0.03; 
//#endif
//
//        // Poll events at the start to process input before rendering
//        glfwPollEvents();
//
//#if 0  // Keyboard handlng is now event driven
//        // Process input (keyboard, mouse, etc.)
//        processInput(window, callbackData.deltaTime);
//#endif
//
//        // Update changing camera variables
//        glm::vec3 cameraPos = camera->Position;
//        view = glm::lookAt(camera->Position, camera->Position + camera->Front, camera->Up);
//
//        // Start the Dear ImGui frame
//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplGlfw_NewFrame();
//        ImGui::NewFrame();
//
//        // TODO: Add function to reset game state, so that when going back to home screen, the game is reset
//        // TODO: Who's supposed to set currentState? loading state is never set.
//
//        if (gameControl.currentState == ProgramState::LOADING) {
//            showLoadingScreen(window, backgroundTexture);
//        }
//        else if (gameControl.showHomeScreen) {
//            // Note: for better performance, distribute these changes to depth test when switching variables
//            glDisable(GL_DEPTH_TEST);
//            if (gameControl.showCustomizeScreen || gameControl.showAboutScreen) {
//                if (gameControl.showCustomizeScreen) {
//                    showCustomizeScreen(window, backgroundTexture);
//                }
//                else if (gameControl.showAboutScreen) {
//                    showAboutScreen(window, backgroundTexture);
//                }
//            }
//            else {
//                showHomeScreen(window, homeScreenTexture, backgroundTexture);
//            }
//        }
//        else {
//            glEnable(GL_DEPTH_TEST);
//
//            // Only update if the game is active
//            if (!gameControl.showOptionsScreen && gameControl.currentState == ProgramState::ACTIVE) {
//                physicsWorld->update(gameControl.deltaTime);
//            }
//
//            if (gameControl.showInfoScreen) {
//                showInfoScreen(window, &imguiColor);
//            }
//
//            // Clear the color and depth buffers to prepare for a new frame
//
//            glClearColor(imguiColor[0], imguiColor[1], imguiColor[2], 1.00f);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//            // Use the shader program (objectShader) for rendering 3D objects, sets viewPos and view
//
//            objectShader->use();
//            objectShader->updateCameraPosition(cameraPos, view);
//            objectShader->setUniformMat4("model", identity4);
//
//            // Render the floor
//
//            glActiveTexture(GL_TEXTURE0);
//            smallHexagonPattern.use();
//            glBindVertexArray(floorVAO);
//            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
//
//            // Update and render the stadium (uses this texture)
//            stadium->render(*objectShader);  // render(shader, light color=white, light position)
//
//            // Update and render the Beyblade
//            // TODO: This should be handled renderAll or something! Maybe physicsWorld should hold the objects and not their rigidBodies to allow for this
//            beyblade1->render(*objectShader);
//            beyblade2->render(*objectShader);
//
//            // Render bounding boxes for debugging
//
//            if (gameControl.debugMode) {
//                physicsWorld->renderDebug(*objectShader);
//                //mainCamera.body->renderDebug(*objectShader, camera->Position);
//                //stadium.body->renderDebug(*objectShader, camera->Position);
//            }
//
//            // TODO: Move this into info screen
//            std::stringstream ss;
//            ss << std::fixed << std::setprecision(1);
//            ss << "X: " << camera->Position.x << " "
//                << "Y: " << camera->Position.y << " "
//                << "Z: " << camera->Position.z << "   |   "
//                << "Min" << camera->body->boundingBoxes[0]->min.x << " " <<
//                camera->body->boundingBoxes[0]->min.y << " " <<
//                camera->body->boundingBoxes[0]->min.z << " "
//                << "Max" << camera->body->boundingBoxes[0]->max.x << " " <<
//                camera->body->boundingBoxes[0]->max.y << " " <<
//                camera->body->boundingBoxes[0]->max.z;
//            std::string cameraPosStr = ss.str();
//            std::replace(cameraPosStr.begin(), cameraPosStr.end(), '-', ';');
//
//            // Render the camera position text
//            glfwGetWindowSize(window, &windowWidth, &windowHeight);
//            textRenderer.Resize(windowWidth, windowHeight);
//            textRenderer.RenderText(cameraPosStr, 25.0f, windowHeight - 50.0f, 0.6f, glm::vec3(0.5f, 0.8f, 0.2f));
//        }
//
//        // Render options screen last
//        if (gameControl.showOptionsScreen) {
//            showOptionsScreen(window);
//        }
//
//        // Render ImGui on top of the 3D scene
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//        // Swap buffers at the end
//        glfwSwapBuffers(window);
//    }
//
//    /* ----------------------CLEANUP-------------------------- */
//
//    // Variables cleanup
//    glDeleteVertexArrays(1, &floorVAO);
//    glDeleteBuffers(1, &floorVBO);
//    glDeleteBuffers(1, &floorEBO);
//
//    // Clean up
//    cleanup(window);
//
//    // Cleanup textures
//    hexagonPattern.cleanup();
//    smallHexagonPattern.cleanup();
//
//    // Cleanup ImGui
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
//
//    return 0;
}