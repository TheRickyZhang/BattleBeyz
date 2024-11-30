#include "UI.h"
#include "GameEngine.h"
#include "ActiveState.h"
#include "StateIdentifiers.h"
using namespace glm;

void ActiveState::init()
{
    // Set up keyboard and mouse callbacks.
    GLFWwindow* window = game->getWindow();

    PhysicsWorld* physicsWorld = game->physicsWorld;

    // Initialize objects for the physics world
    //GLuint floorVAO, floorVBO, floorEBO; // TOLOOK: Make infinite? Like pass in world position and calculate positioning dynamically in the shader
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
    //auto meshBey1 = new BeybladeMesh(beyblade1Path, 0, 0, 0, vec3(1.0f, 1.0f, 1.0f));
    //Layer layer1; Disc disc1; Driver driver1;
    //auto rigidBey1 = new BeybladeBody(meshBey1, layer1, disc1, driver1);
    //Beyblade* beyblade1 = new Beyblade(101, "Beyblade 1", rigidBey1, meshBey1);

    //std::string beyblade2Path = "./assets/models/TestBlade6.obj";
    //auto meshBey2 = new BeybladeMesh(beyblade2Path, 0, 0, 0, vec3(1.0f, 1.0f, 1.0f));
    //Layer layer2; Disc disc2; Driver driver2;
    //auto rigidBey2 = new BeybladeBody(meshBey2, layer2, disc2, driver2);
    //Beyblade* beyblade2 = new Beyblade(102, "Beyblade 2", rigidBey2, meshBey2);

    // These might be null for now, quell errors
    Beyblade* beyblade1 = game->pm.getActiveProfile()->getBeyblade(1).get();
    Beyblade* beyblade2 = game->pm.getActiveProfile()->getBeyblade(2).get();

    //vec3 initialPosition1 = vec3(0.0f, 2.0f, 0.5f);
    //vec initialPosition2 = vec3(0.0f, 2.0f, -0.5f);
    //vec3 initialVelocity1 = vec3(0.0f, 0.0f, -0.2f);
    //vec3 initialVelocity2 = vec3(0.2f, 0.0f, 0.1f);
    //vec3 initialAngularVelocity = vec3(0.0f, -450.0f, 0.0f);
    //beyblade1->getRigidBody()->setInitialLaunch(initialPosition1, initialVelocity1, initialAngularVelocity);
    //beyblade2->getRigidBody()->setInitialLaunch(initialPosition2, initialVelocity2, initialAngularVelocity);

    // 2024-11-18. Reset various things before [re]starting the game.

    physicsWorld->resetPhysics();
    beyblades.clear();

    beyblade1->getRigidBody()->resetPhysics();
    beyblade2->getRigidBody()->resetPhysics();

    beyblades.push_back(beyblade1);  // TODO: 2024-11-18.  Is the extra vector really necessary?
    beyblades.push_back(beyblade2);

    for (Stadium* stadium : stadiums) physicsWorld->addStadium(stadium);
    for (Beyblade* beyblade : beyblades) physicsWorld->addBeyblade(beyblade);
}

void ActiveState::cleanup()
{
}

void ActiveState::pause() {}

void ActiveState::resume() {}

void ActiveState::handleEvents() {
    auto& inputManager = game->inputManager;

    if (inputManager.keyJustPressed(GLFW_KEY_TAB)) {
        showInfoScreen = !showInfoScreen;
    }

    if (inputManager.keyJustPressed(GLFW_KEY_D) && glfwGetKey(game->getWindow(), GLFW_KEY_LEFT_CONTROL)) {
        game->debugMode = !game->debugMode;
        std::cout << "Debug mode is " << (game->debugMode ? "On" : "Off") << std::endl;
    }

    if (inputManager.keyJustPressed(CtrlD)) {
        game->debugMode = !game->debugMode;
        std::cout << "Debug mode is " << (game->debugMode ? "On" : "Off") << std::endl;
    }

    for (const auto& [movementKey, action] : movementKeys) {
        if (inputManager.keyPressed(movementKey)) {
            game->camera->processKeyboard(action, game->deltaTime);
        }
    }
    game->camera->update(game->deltaTime);

    if (inputManager.mouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        double xpos, ypos;
        glfwGetCursorPos(game->getWindow(), &xpos, &ypos);

        // Calculate world ray and perform intersection check
        vec3 ray_world = screenToWorldCoordinates(game->getWindow(), xpos, ypos,
            game->camera->getViewMatrix(),
            game->projection);
        std::cout << "Left mouse button clicked! Ray: " << ray_world[0] << ", "
            << ray_world[1] << ", " << ray_world[2] << std::endl;
    }

    // RIght click + drag to move camera
    if (inputManager.mouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        auto [xOffset, yOffset] = inputManager.getMouseOffsets();
        if (xOffset != 0.0f || yOffset != 0.0f) {
            game->camera->processMouseMovement(xOffset, yOffset);
        }
    }

    if (inputManager.scrollMoved()) {
        float scrollOffsetY = inputManager.getScrollOffsetY();
        game->camera->processMouseScroll(scrollOffsetY);
        inputManager.resetScrollOffset();
    }
}

void ActiveState::update(float deltaTime) {
    game->physicsWorld->update(deltaTime);
}




void ActiveState::draw() {
    ShaderProgram* objectShader = game->objectShader;
    TextureManager& tm = game->tm;

    SetWindowPositionAndSize(3, 4, 1, 1);

    ImGui::Begin("Controls");
    if (ImGui::Button("Back to Home##Active")) {
        game->changeState(GameStateType::HOME);
        ImGui::End();
        return;
        //TODO: Is there any better way to do this than adding end and return here? Otehrwise crash. Also does not preserve state correctly
        // 2024-11-03.  Huh?  Why not leave here?  The 
    }

    glEnable(GL_DEPTH_TEST);

    // Clear the color and depth buffers to prepare for a new frame
    glClearColor(imguiColor[0], imguiColor[1], imguiColor[2], 1.00f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update changing camera variables
    Camera* camera = game->camera;
    vec3 cameraPos = camera->position;
    mat4 view = lookAt(camera->position, camera->position + camera->front, camera->up);

    // Use the shader program (objectShader) for rendering 3D objects, sets viewPos and view
    objectShader->use();
    objectShader->updateCameraPosition(cameraPos, view);
    objectShader->setUniformMat4("model", mat4(1.0));

    // Render the floor
    // TODO: inspect texture loaidng. This should be abstracted into the texture manager
    glActiveTexture(GL_TEXTURE0);
    tm.getTexture("floor")->use(); // SHould use texture, used to be small hexagon pattern
    glBindVertexArray(floorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // update and render the stadium (uses this texture)
    for(auto stadium : stadiums) stadium->render(*objectShader);  // render(shader, light color=white, light position)
    for (auto beyblade : beyblades) beyblade->render(*objectShader);

    // Render the position
    vec3 cameraPosition = game->camera->position;
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1)
        << "Position: " << cameraPosition.x << ", "
        << cameraPosition.y << ", " << cameraPosition.z;
    std::string positionText = ss.str();

    // Only resize if window dimensions have changed
    if (game->windowWidth != game->lastWidth || game->windowHeight != game->lastHeight) {
        game->textRenderer->resize(game->windowWidth, game->windowHeight);
        game->lastWidth = game->windowWidth;
        game->lastHeight = game->windowHeight;
    }

    //float textWidth = ImGui::CalcTextSize(positionText.data()).x;
    game->textRenderer->renderText(
        positionText,
        game->windowWidth - 300.0f,
        game->windowHeight - 20.0f,
        0.5f,
        vec3(1.0f, 1.0f, 1.0f)
    );

    if (game->debugMode) {
        game->physicsWorld->renderDebug(*objectShader);
    }

    if (showInfoScreen) {
        drawInfoScreen();
    }

    ImGui::Text("Use WASDQE for camera movement");
    ImGui::Text("Use Right Click Hold + Drag for camera rotation");
    ImGui::Text("Use the scroll wheel to control moving speed");
    ImGui::End();
}


void ActiveState::drawInfoScreen() {
    SetWindowPositionAndSize(3, 4, 2, 1, 2, 1);

    ImGui::Begin("Settings and Launch Menu");

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
            vec3 initialVelocity = beybladeBody->getVelocity();
            ImGui::SliderFloat("X##V", &initialVelocity.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Y##V", &initialVelocity.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Z##V", &initialVelocity.z, -100.0f, 100.0f);

            ImGui::Text("Center");
            vec3 initialCenter = beybladeBody->getCenter();
            ImGui::SliderFloat("X##CTR", &initialCenter.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Y##CTR", &initialCenter.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Z##CTR", &initialCenter.z, -100.0f, 100.0f);

            ImGui::Text("Angular Velocity");
            vec3 initialAngularVelocity = beybladeBody->getAngularVelocity();
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

    CameraMode currentMode = game->camera->activeMode;
    if (ImGui::RadioButton("Free", currentMode == CameraMode::FREE)) {
        game->camera->changeCameraMode(CameraMode::FREE);
        game->messageLog->open();
        game->messageLog->addMessage("Camera changed to free", MessageType::NORMAL);
    }
    if (ImGui::RadioButton("Attached", currentMode == CameraMode::ATTACHED)) {
        game->camera->changeCameraMode(CameraMode::ATTACHED);
        game->messageLog->open();
        game->messageLog->addMessage("Camera attached to bey" + std::string(game->pm.getActiveProfile()->getName()), MessageType::NORMAL);
    }
    if (ImGui::RadioButton("Panning", currentMode == CameraMode::PANNING)) {
        game->camera->changeCameraMode(CameraMode::PANNING);
        game->messageLog->open();
        game->messageLog->addMessage("Camera changed to pan", MessageType::NORMAL);
    }

    ImGui::End();
}