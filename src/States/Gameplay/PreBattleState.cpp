#include "PreBattleState.h"

#include "GameEngine.h"
#include "PhysicsWorld.h"
#include "Buffers.h"
//#include "StateIdentifiers.h"
#include "ObjectShader.h"
#include "ImGuiUI.h"
#include "Camera.h"
#include "InputManager.h"
#include "GameMessage.h"
#include "ProfileManager.h"
#include "TextureManager.h"
#include "MessageLog.h"

#include "InteractiveBehavior.h"
#include "Utils.h"

using namespace std;
using namespace glm;


void PreBattleState::init()
{
    // Set up keyboard and mouse callbacks.
    GLFWwindow* window = game->getWindow();
    PhysicsWorld* physicsWorld = game->physicsWorld;

    this->floor = new Floor(1000.0f, 1000.0f, 0.0f, 0.0f, 0.0f);
    //floor->setTextureScale(vec2(100.0f, 100.0f));


    physicsWorld->resetPhysics();

    float xmin = FLT_MAX, xmax = FLT_MIN, zmin = FLT_MAX, zmax = FLT_MIN, ymin = FLT_MAX; 

    if (stadiums.empty()) cout << "HUUUUUHHHHHH???" << endl;
    for (shared_ptr<Stadium> stadium : stadiums) {
        // Add physics
        physicsWorld->addStadium(stadium.get());

        // Adjust camera boundaries rectangularly based on stadium
        float x = stadium->getCenter().x();
        float y = stadium->getCenter().y();
        float z = stadium->getCenter().z();
        float r = stadium->getRadius().value();
        xmin = std::min(xmin, x - r);
        xmax = std::max(xmax, x + r);
        zmin = std::min(zmin, z - r);
        zmax = std::max(zmax, z + r);
        ymin = std::min(ymin, y);
    }
    for (shared_ptr<Beyblade> beyblade : beyblades) {
        beyblade->getBody()->resetPhysics(Vec3_M(0.0f, 1.0f, 0.0f));
        physicsWorld->addBeyblade(beyblade.get());
    }
}

void PreBattleState::cleanup()
{
   
}

void PreBattleState::pause() {}

void PreBattleState::resume() {}

void PreBattleState::handleEvents() {
    GLFWwindow* window = game->getWindow();
    Camera* camera = game->camera;

    // --- Picking Phase: On initial left click, try to select a beyblade ---
    if (game->im.mouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT) && heldBeyblade == nullptr) {
        double dx, dy;
        glfwGetCursorPos(window, &dx, &dy);
        float xpos = static_cast<float>(dx);
        float ypos = static_cast<float>(dy);

        glm::vec3 rayDir = screenToWorldCoordinates(window, xpos, ypos,
            camera->getViewMatrix(),
            game->projection);
        rayDir = glm::normalize(rayDir);
        glm::vec3 rayOrigin = camera->position;

        Beyblade* selectedBeyblade = nullptr;
        glm::vec3 hitIntersection;
        float closestT = FLT_MAX;

        for (std::shared_ptr<Beyblade> beyblade : beyblades) {
            float t;
            if (rayIntersectsAABB(rayOrigin, rayDir, beyblade->getBody()->getBoundingBox(), t)) {
                if (t < closestT) {
                    closestT = t;
                    selectedBeyblade = beyblade.get();
                    hitIntersection = rayOrigin + t * rayDir;
                }
            }
        }

        if (selectedBeyblade) {
            heldBeyblade = selectedBeyblade;
            // Save offset so the object “sticks” relative to the pointer
            dragOffset = heldBeyblade->getBody()->getCenter().value() - hitIntersection;
        }
    }

    // --- Dragging Phase: While left button is held, move the beyblade ---
    if (game->im.mouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && heldBeyblade != nullptr) {
        double dx, dy;
        glfwGetCursorPos(window, &dx, &dy);
        float xpos = static_cast<float>(dx);
        float ypos = static_cast<float>(dy);

        // Compute a new ray from the camera through the current mouse position.
        glm::vec3 rayDir = screenToWorldCoordinates(window, xpos, ypos,
            camera->getViewMatrix(),
            game->projection);
        rayDir = glm::normalize(rayDir);
        glm::vec3 rayOrigin = camera->position;

        // Compute intersection with the stadium floor (a horizontal plane at stadiumY).
        // Guard against nearly horizontal rays:
        const float epsilon = 1e-6f;
        if (std::fabs(rayDir.y) > epsilon) {
            float t = (stadiumY - rayOrigin.y) / rayDir.y;
            glm::vec3 currentIntersection = rayOrigin + t * rayDir;

            // New beyblade center preserves the initial drag offset.
            glm::vec3 newBeybladePos = currentIntersection + dragOffset;
            heldBeyblade->getBody()->setCenter(newBeybladePos);
        }
    }

    // --- Release Phase: When left mouse button is released, stop dragging ---
    if (game->im.mouseButtonJustReleased(GLFW_MOUSE_BUTTON_LEFT)) {
        heldBeyblade = nullptr;
    }

    // ... existing camera movement, UI, etc.

    // Clamp camera to area bounded by stadium, +y
    //camera->enforceBoundaries();
    camera->update(game->deltaTime);
    camera->handleMouseDrag(game->im);
    camera->handleMouseScroll(game->im);

    // No debugging for now
    
    //if (game->im.keyJustPressed(GLFW_KEY_TAB)) {
    //    showInfoScreen = !showInfoScreen;
    //}
    //if (game->im.keyJustPressed(GLFW_KEY_D) && glfwGetKey(game->getWindow(), GLFW_KEY_LEFT_CONTROL)) {
    //    game->debugMode = !game->debugMode;
    //    std::cout << "Debug mode is " << (game->debugMode ? "On" : "Off") << std::endl;
    //}
    //if (game->im.keyJustPressed(CtrlD)) {
    //    game->debugMode = !game->debugMode;
    //    std::cout << "Debug mode is " << (game->debugMode ? "On" : "Off") << std::endl;
    //}

    // Take input first, custom?
    //for (const auto& [movementKey, action] : movementKeys) {
    //    if (game->im.keyPressed(movementKey)) {
    //        game->camera->processKeyboard(action, game->deltaTime);
    //    }
    //}
}

void PreBattleState::onResize(int width, int height) {}

void PreBattleState::update(float deltaTime) {
    game->physicsWorld->update(deltaTime);
}


void PreBattleState::draw() {
    ObjectShader* objectShader = game->objectShader;
    TextureManager& tm = game->tm;

    SetWindowPositionAndSize(3, 4, 1, 1);

    ImGui::Begin("Controls");
    if (ImGui::Button("Back to Home##Active")) {
        ImGui::End();
        game->changeState(StateFactory::createState(game, GameStateType::HOME));
        return;
    }

    glEnable(GL_DEPTH_TEST);

    // Clear the color and depth buffers to prepare for a new frame
    glClearColor(imguiColor[0], imguiColor[1], imguiColor[2], 1.00f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update changing camera variables
    Camera* camera = game->camera;
    vec3 cameraPos = camera->position;
    mat4 view = lookAt(camera->position, camera->position + camera->front, camera->up);

    // IMPORTANT: Once per frame!!!!! ONLY model should be passed into the gameobject
    // Use the shader program (objectShader) for rendering 3D objects, sets viewPos and view
    objectShader->use();
    // objectShader->setCameraView(cameraPos, view);
    objectShader->setGlobalRenderParams(view, game->projection, cameraPos);


    floor->render(*game->objectShader, tm.getTexture("floor").get());

    for (const std::shared_ptr<Stadium>& stadium : stadiums) {
        stadium->render(*objectShader);
    }
    for (const shared_ptr<Beyblade> beyblade : beyblades) beyblade->render(*objectShader);


    // Render the position
    vec3 cameraPosition = game->camera->position;
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1)
        << "Position: " << cameraPosition.x << ", "
        << cameraPosition.y << ", " << cameraPosition.z;
    std::string positionText = ss.str();

    if (game->debugMode) {
        game->physicsWorld->renderDebug(*objectShader);
    }

    if (showInfoScreen) {
        drawInfoScreen();
    }

    ImGui::Text("WASDQE: camera movement");
    ImGui::Text("Right Click + Drag: camera rotation");
    ImGui::Text("Scroll wheel: movement speed");
    ImGui::Text((positionText).c_str());
    ImGui::End();
}


void PreBattleState::drawInfoScreen() {
    SetWindowPositionAndSize(3, 4, 2, 1, 2, 1);

    ImGui::Begin("Settings and Launch Menu");

    // Pause button
    ImGui::SameLine();
    if (ImGui::Button("Pause")) {
        ImGui::End();
        game->changeState(StateFactory::createState(game, GameStateType::PAUSE));
        return;
    }

    // Exit button
    ImGui::SameLine();
    //renderExitButton(window);

    // Home Button
    ImGui::SameLine();
    if (ImGui::Button("Home")) {
        ImGui::End();
        game->changeState(StateFactory::createState(game, GameStateType::HOME));
        return;
    }

    ImGui::Separator();

    for (const shared_ptr<Beyblade>& beyblade : beyblades) {
        BeybladeBody* beybladeBody = beyblade->getBody();
        if (ImGui::CollapsingHeader(beyblade->getName().data())) {
            ImGui::Text("Velocity");
            vec3 initialVelocity = beybladeBody->getVelocity().value();
            ImGui::SliderFloat("X##V", &initialVelocity.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Y##V", &initialVelocity.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Z##V", &initialVelocity.z, -100.0f, 100.0f);

            ImGui::Text("Center");
            vec3 initialCenter = beybladeBody->getCenter().value();
            ImGui::SliderFloat("X##CTR", &initialCenter.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Y##CTR", &initialCenter.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Z##CTR", &initialCenter.z, -100.0f, 100.0f);

            ImGui::Text("Angular Velocity");
            vec3 initialAngularVelocity = beybladeBody->getAngularVelocity().value();
            ImGui::SliderFloat("X##AV", &initialAngularVelocity.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Y##AV", &initialAngularVelocity.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Z##AV", &initialAngularVelocity.z, -100.0f, 100.0f);

            if (ImGui::Button("Apply Launch Settings")) {
                beybladeBody->setInitialLaunch(Vec3_M(initialCenter), initialVelocity, initialAngularVelocity);
            }
        }
    }

    if (ImGui::Button("Launch")) {

    }

    ImGui::Checkbox("Bound Camera", &game->boundCamera);

    CameraMode currentMode = game->camera->activeMode;
    if (ImGui::RadioButton("Free", currentMode == CameraMode::FREE)) {
        game->camera->changeCameraMode(CameraMode::FREE);
        game->ml.addMessage("Camera changed to free", MessageType::NORMAL, true);
    }
    if (ImGui::RadioButton("Attached", currentMode == CameraMode::ATTACHED)) {
        game->camera->changeCameraMode(CameraMode::ATTACHED);
        game->ml.addMessage("Camera attached to bey" + std::string(game->pm.getActiveProfile()->getName()), MessageType::NORMAL, true);
    }
    if (ImGui::RadioButton("Panning", currentMode == CameraMode::PANNING)) {
        game->camera->changeCameraMode(CameraMode::PANNING);
        game->ml.addMessage("Camera changed to pan", MessageType::NORMAL, true);
    }

    ImGui::End();
}