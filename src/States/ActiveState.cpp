#include "../UI.h"
#include "../GameEngine.h"
#include "ActiveState.h"
#include "StateIdentifiers.h"

void ActiveState::init()
{
    // Set up keyboard and mouse callbacks.
    GLFWwindow* window = game->getWindow();
    glfwSetKeyCallback(window, ActiveState::keyCallback);
    glfwSetMouseButtonCallback(window, ActiveState::mouseButtonCallback);
    glfwSetCursorPosCallback(window, ActiveState::mouseCallback);
    glfwSetScrollCallback(window, ActiveState::scrollCallback);

    PhysicsWorld* physicsWorld = game->physicsWorld;

    // Initialize objects for the physics world
    //GLuint floorVAO, floorVBO, floorEBO; // TODO: Make infinite? Like pass in world position and calculate positioning dynamically in the shader
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

    GLuint stadiumVAO = 0, stadiumVBO = 0, stadiumEBO = 0;

    // TODO: this should add stadiums dynamically in future, but use default single one for now
    StadiumBody* rigidBody = new StadiumBody();
    StadiumMesh* stadiumMesh = new StadiumMesh();
    Stadium* stadium1 = new Stadium(rigidBody, stadiumMesh, "Stadium 1");
    stadiums.push_back(stadium1);

    // NEWMESH: Load meshes before bodies so we can get the actual object sizes. Then pass the
    // mesh to the BeybladdeBody constructor.
    // NEWMESH: remove radius and heigth from these objects, leaving just some physics coefficients.
    // TODO: Ensure that the VAOs / VBOs are handled withing the mesh objects properly (ex we can probably remove passing in 0 parameters)

    //std::string beyblade1Path = "./assets/models/TestBlade5.obj";
    //auto meshBey1 = new BeybladeMesh(beyblade1Path, 0, 0, 0, glm::vec3(1.0f, 1.0f, 1.0f));
    //Layer layer1; Disc disc1; Driver driver1;
    //auto rigidBey1 = new BeybladeBody(meshBey1, layer1, disc1, driver1);
    //Beyblade* beyblade1 = new Beyblade(101, "Beyblade 1", rigidBey1, meshBey1);

    //std::string beyblade2Path = "./assets/models/TestBlade6.obj";
    //auto meshBey2 = new BeybladeMesh(beyblade2Path, 0, 0, 0, glm::vec3(1.0f, 1.0f, 1.0f));
    //Layer layer2; Disc disc2; Driver driver2;
    //auto rigidBey2 = new BeybladeBody(meshBey2, layer2, disc2, driver2);
    //Beyblade* beyblade2 = new Beyblade(102, "Beyblade 2", rigidBey2, meshBey2);

    // These might be null for now, quell errors
    Beyblade* beyblade1 = game->pm.getActiveProfile()->getBeyblade(1).get();
    Beyblade* beyblade2 = game->pm.getActiveProfile()->getBeyblade(2).get();

    //glm::vec3 initialPosition1 = glm::vec3(0.0f, 2.0f, 0.5f);
    //glm::vec initialPosition2 = glm::vec3(0.0f, 2.0f, -0.5f);
    //glm::vec3 initialVelocity1 = glm::vec3(0.0f, 0.0f, -0.2f);
    //glm::vec3 initialVelocity2 = glm::vec3(0.2f, 0.0f, 0.1f);
    //glm::vec3 initialAngularVelocity = glm::vec3(0.0f, -450.0f, 0.0f);
    //beyblade1->getRigidBody()->setInitialLaunch(initialPosition1, initialVelocity1, initialAngularVelocity);
    //beyblade2->getRigidBody()->setInitialLaunch(initialPosition2, initialVelocity2, initialAngularVelocity);

    beyblades.push_back(beyblade1);
    beyblades.push_back(beyblade2);

    for (Stadium* stadium : stadiums) physicsWorld->addStadium(stadium);
    for (Beyblade* beyblade : beyblades) physicsWorld->addBeyblade(beyblade);
}

void ActiveState::cleanup()
{
    // Remove keyboard and mouse callbacks.

    GLFWwindow* window = game->getWindow();

    glfwSetKeyCallback(window, nullptr);
    glfwSetMouseButtonCallback(window, nullptr);
    glfwSetCursorPosCallback(window, nullptr);
    glfwSetScrollCallback(window, nullptr);
}

void ActiveState::pause() {}

void ActiveState::resume() {}

void ActiveState::handleEvents() {}

void ActiveState::update(float deltaTime) {
    // TODO: Clarify this usage
    if (input.isKeyPressed(GLFW_KEY_W)) {
        // Move forward
    }
    if (input.isKeyPressed(GLFW_KEY_S)) {
        // Move backward
    }
    if (input.isKeyPressed(GLFW_KEY_ESCAPE)) {
        // Pause game or open options menu
        gameEngine->pushState(std::make_unique<PauseState>());
    }

    // Handle mouse input
    if (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        // Fire weapon or interact
    }

    // Handle mouse movement
    glm::vec2 mousePos = input.getMousePosition();
    // Use mousePos for camera rotation or object selection

    // Handle scroll input
    glm::vec2 scrollOffset = input.getScrollOffset();
    if (scrollOffset.y != 0.0f) {
        // Zoom in or out
    }
    // Reset scroll offset after processing
    input.resetScrollOffset();

    game->physicsWorld->update(deltaTime);
}

void ActiveState::drawInfoScreen() {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings and Launch Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Pause button
    ImGui::SameLine();
    if (ImGui::Button("Pause")) {
        game->pushState(GameStateType::PAUSE);
    }

    // Exit button
    ImGui::SameLine();
    //renderExitButton(window);

    // Home Button
    ImGui::SameLine();
    if (ImGui::Button("Home")) {
        game->pushState(GameStateType::HOME);
    }

    ImGui::Separator();

    for (Beyblade* beyblade : beyblades) {
        BeybladeBody* beybladeBody = beyblade->getRigidBody();
        if (ImGui::CollapsingHeader(beyblade->getName().data())) {
            ImGui::Text("Velocity");
            glm::vec3 initialVelocity = beybladeBody->getVelocity();
            ImGui::SliderFloat("X##V", &initialVelocity.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Y##V", &initialVelocity.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Z##V", &initialVelocity.z, -100.0f, 100.0f);

            ImGui::Text("Center");
            glm::vec3 initialCenter = beybladeBody->getCenter();
            ImGui::SliderFloat("X##CTR", &initialCenter.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Y##CTR", &initialCenter.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Z##CTR", &initialCenter.z, -100.0f, 100.0f);

            ImGui::Text("Angular Velocity");
            glm::vec3 initialAngularVelocity = beybladeBody->getAngularVelocity();
            ImGui::SliderFloat("X##AV", &initialAngularVelocity.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Y##AV", &initialAngularVelocity.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Z##AV", &initialAngularVelocity.z, -100.0f, 100.0f);

            if (ImGui::Button("Apply Launch Settings")) {
                beybladeBody->setInitialLaunch(initialCenter, initialVelocity, initialAngularVelocity);
            }
        }
    }

    if (ImGui::Button("Launch")) {

    }

    ImGui::Checkbox("Bound Camera", &game->boundCamera);

    ImGui::End();
}



void ActiveState::draw() {
    ShaderProgram* objectShader = game->objectShader;
    TextureManager& tm = game->tm;

    ImGui::Begin("About");
    if (ImGui::Button("Back to Home")) {
        game->changeState(GameStateType::HOME);
    }

    glEnable(GL_DEPTH_TEST);

    // Clear the color and depth buffers to prepare for a new frame
    glClearColor(imguiColor[0], imguiColor[1], imguiColor[2], 1.00f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update changing camera variables
    Camera* camera = game->cameraState->camera;
    glm::vec3 cameraPos = camera->position;
    glm::mat4 view = glm::lookAt(camera->position, camera->position + camera->front, camera->up);

    // Use the shader program (objectShader) for rendering 3D objects, sets viewPos and view
    objectShader->use();
    objectShader->updateCameraPosition(cameraPos, view);
    objectShader->setUniformMat4("model", glm::mat4(1.0));

    // Render the floor
    // TODO: inspect texture loaidng. This should be abstracted into the texture manager
    glActiveTexture(GL_TEXTURE0);
    tm.getTexture("floor")->use(); // SHould use texture, used to be small hexagon pattern
    glBindVertexArray(floorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // update and render the stadium (uses this texture)
    for(auto stadium : stadiums) stadium->render(*objectShader);  // render(shader, light color=white, light position)
    for (auto beyblade : beyblades) beyblade->render(*objectShader);

    if (game->debugMode) {
        game->physicsWorld->renderDebug(*objectShader);
    }

    if (showInfoScreen) {
        drawInfoScreen();
    }

    ImGui::Text("This is the Active screen.");
    ImGui::End();
}


/**
* Keyboard callback.
*
* @param window                 [in] The parent window.
*
* @param key                    [in] The GLFW key code such as the letter 'A'.
*
* @param scancode               [in] The raw key number.  For example, the cursor
*                               keys do not correspond to letters.
*
* @param action                 [in] The key action that occured (press, release,
*                               or release).
*/

/* static. TOLOOK: Since we already have window defined in gameEngine, is there a way to reduce redundant calls? */
void ActiveState::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    auto* game = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
    if (!game) return;

    auto gs = game->getGameState();
    if (gs && gs->getStateType() == GameStateType::ACTIVE) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            switch (key) {
            case GLFW_KEY_TAB:  // Toggle display of the "Info Screen".
                //?? TODO ?? engine->showInfoScreen = !engine->showInfoScreen;
                break;
            case GLFW_KEY_D:
                if ((mods & GLFW_MOD_CONTROL)) {  // CTRL-D: toggle debug mode.
                    game->debugMode = !game->debugMode;
                    std::cout << "Debug mode is " << (game->debugMode ? "On" : "Off") << std::endl;
                    break;
                }  // Else fall into code below
            case GLFW_KEY_A:
            case GLFW_KEY_E:
            case GLFW_KEY_S:
            case GLFW_KEY_Q:
            case GLFW_KEY_I:
                showInfoScreen = !showInfoScreen;
            case GLFW_KEY_W:
                game->cameraState->camera->processKeyboard(key, game->deltaTime, game->boundCamera);
                break;
            case GLFW_KEY_ESCAPE:
                // Toggle Options screen when ESC is pressed, but ensure it only happens on one press
                if (!escPressed) {
                    //?? TODO ?? engine->showOptionsScreen = !engine->showOptionsScreen;
                    escPressed = true;
                }
                break;
            case GLFW_KEY_UNKNOWN:  // We don't use this key.
                break;
            }
        }
    }

    // Handle key release to reset the escPressed flag
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_ESCAPE) {
            escPressed = false;
        }
    }
}

/**
* Mouse button handler.
*/

/* TODO: make static */
void ActiveState::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

         //TODO: Refactor, as GameControl is no longer valid
        glm::vec3 ray_world = screenToWorldCoordinates(window, xpos, ypos, game->cameraState->camera->getViewMatrix(), game->projection);
        std::string clickedObject = checkIntersection(ray_world);
        std::cout << "Left mouse button clicked! Ray: " << ray_world[0] << ", " << ray_world[1] << ", " << ray_world[2] << " Object: " << clickedObject << std::endl;
    }
}

/**
* Mouse movement callback.
*
* This adjust the viewport when the window is resized, and moves the
* camera when the right mouse button is pressed.
*/

/* Not static - depends on the current */
void ActiveState::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
    ImGuiIO& io = ImGui::GetIO();

    // Process camera movement only when the right mouse button is pressed and ImGui is not capturing the mouse
    if (!io.WantCaptureMouse || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        auto* game = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
        if (!game || !game->cameraState) return;
        CameraState* cameraState = game->cameraState;

        if (cameraState->firstMouse) {
            cameraState->lastX = xpos;
            cameraState->lastY = ypos;
            cameraState->firstMouse = false;
        }

        double xoffset = xpos - cameraState->lastX;
        double yoffset = cameraState->lastY - ypos;
        cameraState->lastX = xpos;
        cameraState->lastY = ypos;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            cameraState->camera->processMouseMovement((float)xoffset, (float)yoffset);
        }
        else {
            cameraState->firstMouse = true;
        }
    }
}

/**
* Scroll handler.
*
* This handles camera speed and zoom.
*/

/* static */
void ActiveState::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    auto* game = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
    if (!game || !game->cameraState) return;
    CameraState* cameraState = game->cameraState;

    cameraState->camera->processMouseScroll((float)yoffset);
}
