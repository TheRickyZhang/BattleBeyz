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

//    // Window dimensions
//#if 0
//    int windowWidth = 1600, windowHeight = 900;
//    const float aspectRatio = 16.0f / 9.0f;
//#else  // TODO: This way will never work on a phone!
//    // 1600:900 is a 16:9 aspect ratio
//    int windowWidth = 1600;
//    int windowHeight = 900;
//    const float aspectRatio = (float)windowWidth / (float)windowHeight;
//#endif
//    int minWidth = windowWidth / 4;
//    int minHeight = static_cast<int>(double(minWidth) / aspectRatio);

    /* ----------------------INITIALIZATION-------------------------- */

    //// Initialize GLFW
    //if (!glfwInit()) {
    //    std::cerr << "Failed to initialize GLFW" << std::endl;
    //    return -1;
    //}

    //// Configure GLFW
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //// Create a GLFW window. Note you NEED to make context current to initialize everything else
    //GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "BattleBeyz", nullptr, nullptr);
    //if (!window) {
    //    std::cerr << "Failed to create GLFW window" << std::endl;
    //    cleanup(window);
    //    return -1;
    //}
    //glfwMakeContextCurrent(window);

    // Center the main window

    //int nMonitors;
    //GLFWmonitor** monitors = glfwGetMonitors(&nMonitors);
    //const GLFWvidmode* videoMode = glfwGetVideoMode(monitors[0]);

    //int monitorX, monitorY;
    //glfwGetMonitorPos(monitors[0], &monitorX, &monitorY);

    //glfwSetWindowPos(window,
    //    monitorX + (videoMode->width - windowWidth) / 2,
    //    monitorY + (videoMode->height - windowHeight) / 2);

    // Initialize GLEW
    //glewExperimental = GL_TRUE;
    //if (glewInit() != GLEW_OK) {
    //    std::cerr << "Failed to initialize GLEW" << std::endl;
    //    cleanup(window);
    //    return -1;
    //}

    //// Set color blinding and depth testing
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glDisable(GL_DEPTH_TEST);

    //// Setup ImGui context
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //// Initialize ImGui fonts
    //ImFont* defaultFont = io.Fonts->AddFontFromFileTTF(DEFAULT_FONT_PATH, 24.0f);
    //ImFont* titleFont = io.Fonts->AddFontFromFileTTF(TITLE_FONT_PATH, 48.0f);
    //ImFont* attackFont = io.Fonts->AddFontFromFileTTF(ATTACK_FONT_PATH, 128.0f);
    //io.Fonts->Build();

    //// Setup Dear ImGui style
    //ImGui::StyleColorsDark();

    //// Setup Platform/Renderer bindings
    //ImGui_ImplGlfw_InitForOpenGL(window, true);
    //ImGui_ImplOpenGL3_Init("#version 330");


    /* ----------------------"GLOBAL" VARIABLES-------------------------- */

    //// Relevant matrices
    //auto model = glm::mat4(1.0f);
    //auto view = glm::mat4(1.0f);
    //auto projection = glm::mat4(1.0f);

    // Declare physics world with default parameters
    auto physicsWorld = new PhysicsWorld();

    //// Primary camera and camera state
    //glm::vec3 initialCameraPos(5.0f, 2.4f, 0.0f);
    //glm::vec3 lookAtPoint(0.0f, 1.0f, 0.0f);
    //glm::vec3 frontVector = glm::normalize(lookAtPoint - initialCameraPos);

    //// Calculate initial yaw and pitch
    //float initialYaw = glm::degrees(std::atan2(frontVector.z, frontVector.x)); // Yaw based on X and Z
    //float initialPitch = glm::degrees(std::atan2(frontVector.y, glm::length(glm::vec2(frontVector.x, frontVector.z))));

    //Camera mainCamera(initialCameraPos, initialYaw, initialPitch, 0.0f, physicsWorld);
    //// TODO? Camera mainCamera(initialCameraPos, initialYaw, initialPitch, 0.0f, nullptr);
    //mainCamera.Front = frontVector;  // Set the front vector to look at the desired point
    //auto cameraState = new CameraState(&mainCamera, 400.0, 300.0);


    //// Time variables
    //float prevTime = 0.0f;

    ////static float imguiColor[3] = {1.0f, 0.0f, 0.0f}; // Red
    //static float imguiColor[3] = { 0.8f, 0.8f, 0.8f }; // Very Light Gray

    /* ----------------------RELEVANT INSTANTIATIONS-------------------------- */


    //auto quadRenderer = new QuadRenderer();

    //auto identity4 = glm::mat4(1.0f);
    //// Identity matrix, starting view, and projection matrices
    //model = identity4;
    //view = glm::lookAt(initialCameraPos, lookAtPoint, glm::vec3(0.0f, 1.0f, 0.0f));
    //projection = glm::perspective(glm::radians(45.0f), float(windowWidth) / float(windowHeight), 0.1f, 100.0f);

    //// Set orthographic projection and depth values for the background shader
    //auto backgroundModel = identity4;
    //auto backgroundView = identity4;
    //glm::mat4 orthoProjection = glm::ortho(0.0f, float(windowWidth), 0.0f, float(windowHeight), 0.0f, 1.0f);

    // Not urgent - position camera at the origin and rotate around it (Would like a background similar to minecraft's homescreen, but from the center of a stadium)

    // Initialize ShaderProgram for 3D objects
    //auto objectShader = new ShaderProgram(OBJECT_VERTEX_SHADER_PATH, OBJECT_FRAGMENT_SHADER_PATH);
    //// Initialize default shader program with the model, view, and projection matrices. Also sets to use.
    //objectShader->setUniforms(model, view, projection);
    //objectShader->setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    //objectShader->setUniformVec3("lightPos", glm::vec3(0.0f, 1e5f, 0.0f)); // Light position very high in the y-direction

    //auto backgroundShader = new ShaderProgram(BACKGROUND_VERTEX_SHADER_PATH, BACKGROUND_FRAGMENT_SHADER_PATH);
    //backgroundShader->setUniforms(backgroundModel, backgroundView, orthoProjection);
    //backgroundShader->setUniform1f("wrapFactor", 4.0f);

    //    auto panoramaShader = new ShaderProgram(PANORAMA_VERTEX_SHADER_PATH, PANORAMA_FRAGMENT_SHADER_PATH);
    //    panoramaShader->setUniforms(panoramaModel, panoramaView, panoramaProjection);

        // Initialize font rendering
    ////TextRenderer textRenderer("./assets/fonts/paladins.ttf", 800, 600);
    //TextRenderer textRenderer("./assets/fonts/OpenSans-Regular.ttf", 800, 600);

    //// Initialize textures. Note that texture1 is primary texture
    //Texture hexagonPattern("./assets/images/Hexagon.jpg", "texture1");
    //Texture smallHexagonPattern("./assets/images/HexagonSmall.jpg", "texture1");
    //auto floorTexture = new Texture("./assets/images/Wood1.jpg", "texture1");
    auto stadiumTexture = new Texture("./assets/textures/Hexagon.jpg", "texture1");

    //// Static texture object
    //Texture homeScreenTexture("./assets/images/Brickbeyz.jpg", "texture1");
    //Texture backgroundTexture("./assets/images/Brickbeyz.jpg", "texture1");
    //std::cout << "Texture ID: " << hexagonPattern.ID << std::endl;
    //std::cout << "Texture ID: " << smallHexagonPattern.ID << std::endl;
    //std::cout << "Texture ID: " << homeScreenTexture.ID << std::endl;
    //std::cout << "Texture ID: " << backgroundTexture.ID << std::endl;

    //// Initialize camera and camera state
    //GameControl gameControl(&windowWidth, &windowHeight, aspectRatio, &projection,
    //    objectShader, backgroundShader, cameraState, quadRenderer,
    //    true, false, false, false, defaultFont,
    //    titleFont, attackFont, false, ProgramState::ACTIVE,
    //    false /* debug mode*/, physicsWorld);

    //// Store the callback data in the window for easy access
    //glfwSetWindowUserPointer(window, &gameControl);

    //// Handle resizing the window
    //glfwSetWindowSizeLimits(window, minWidth, minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    //// Other callbacks
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glfwSetKeyCallback(window, key_callback);
    //glfwSetScrollCallback(window, scroll_callback);

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

    StadiumBody* rigidBody = new StadiumBody(stadiumPosition, stadiumRadius, stadiumCurvature, stadiumCoefficientOfFriction);
    StadiumMesh* stadiumMesh = new StadiumMesh(stadiumTexture, stadiumRadius, sectionsPerRing, numRings, ringColor, crossColor, stadiumColor, stadiumTextureScale);

    Stadium* stadium = new Stadium(rigidBody, stadiumMesh, "Stadium 1");
    physicsWorld->addStadium(stadium);

    // NEWMESH: Load meshes before bodies so we can get the actual object sizes. Then pass the
    // mesh to the BeybladdeBody constructor.
    // Also call mesh->initializeMesh() here.

    GLuint Bey1VAO = 0, Bey1VBO = 0, Bey1EBO = 0;
    std::string beyblade1Path = "./assets/models/TestBlade5.obj";
    auto meshBey1 = new BeybladeMesh(beyblade1Path, Bey1VAO, Bey1VBO, Bey1EBO, glm::vec3(1.0f, 1.0f, 1.0f));
    meshBey1->initializeMesh();

    // NEWMESH: TODO: remove radius and heigth from these objects, leaving just some physics coefficients.
    // Use default constructors for now
    Layer layer1;
    Disc disc1; 
    Driver driver1;
    auto rigidBey1 = new BeybladeBody(meshBey1, layer1, disc1, driver1);

    Beyblade* beyblade1 = new Beyblade(rigidBey1, meshBey1, "Beyblade 1");

    GLuint Bey2VAO = 0, Bey2VBO = 0, Bey2EBO = 0;
    std::string beyblade2Path = "./assets/models/TestBlade6.obj";
    auto meshBey2 = new BeybladeMesh(beyblade2Path, Bey2VAO, Bey2VBO, Bey2EBO, glm::vec3(1.0f, 1.0f, 1.0f));
    meshBey2->initializeMesh();

    Layer layer2;
    Disc disc2;
    Driver driver2;
    auto rigidBey2 = new BeybladeBody(meshBey2, layer2, disc2, driver2);


    Beyblade* beyblade2 = new Beyblade(rigidBey2, meshBey2, "Beyblade 2");

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

    engine.pushState(GameStateType::LOADING);

    while (engine.running()) {
#if 0
        auto currentTime = static_cast<float>(glfwGetTime());
        engine.deltaTime = currentTime - engine.prevTime;
        engine.prevTime = currentTime;
# else
        engine.deltaTime = 0.0052f; // fixed frame rate, lower = slower
#endif

        engine.handleEvents();
        engine.update();
        engine.draw();
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
//
//        glm::vec3 cameraPos = cameraState->camera->Position;
//        view = glm::lookAt(cameraState->camera->Position, cameraState->camera->Position + cameraState->camera->Front, cameraState->camera->Up);
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
//                //mainCamera.body->renderDebug(*objectShader, cameraState->camera->Position);
//                //stadium.body->renderDebug(*objectShader, cameraState->camera->Position);
//            }
//
//            // TODO: Move this into info screen
//            std::stringstream ss;
//            ss << std::fixed << std::setprecision(1);
//            ss << "X: " << cameraState->camera->Position.x << " "
//                << "Y: " << cameraState->camera->Position.y << " "
//                << "Z: " << cameraState->camera->Position.z << "   |   "
//                << "Min" << cameraState->camera->body->boundingBoxes[0]->min.x << " " <<
//                cameraState->camera->body->boundingBoxes[0]->min.y << " " <<
//                cameraState->camera->body->boundingBoxes[0]->min.z << " "
//                << "Max" << cameraState->camera->body->boundingBoxes[0]->max.x << " " <<
//                cameraState->camera->body->boundingBoxes[0]->max.y << " " <<
//                cameraState->camera->body->boundingBoxes[0]->max.z;
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