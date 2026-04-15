#include "ActiveState.h"

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

#include "Utils.h"

using namespace std;
using namespace glm;


void ActiveState::init()
{
    PhysicsWorld* physicsWorld = game->physicsWorld.get();

    this->floor = new Floor(1000.0f, 1000.0f, 0.0f, 0.0f, 0.0f);
    //floor->setTextureScale(vec2(100.0f, 100.0f));

   
    physicsWorld->resetPhysics();
    for (shared_ptr<Stadium> stadium : stadiums) {
        physicsWorld->addStadium(stadium.get());
    }
    for (shared_ptr<Beyblade> beyblade : beyblades) {
        physicsWorld->addBeyblade(beyblade.get());
    }

    //// 2024-11-18. Reset various things before [re]starting the game.
    //// TODO: Screen to modify initial conditions (launch location, angle, speed) beforehand so resetPhysics() works
    //beyblades.clear();

    //beyblade1->getBody()->resetPhysics();
    //beyblade2->getBody()->resetPhysics();

    //beyblades.push_back(beyblade1);  // TODO: 2024-11-18.  Is the extra vector really necessary?
    //beyblades.push_back(beyblade2);

    //for (const std::shared_ptr<Stadium>& stadium : stadiums) {
    //    physicsWorld->addStadium(stadium.get());
    //}
    //for (Beyblade* beyblade : beyblades) physicsWorld->addBeyblade(beyblade);

    if(!beyblades.empty()) game->camera->setFollowingBey(beyblades[0]);
    if(!stadiums.empty()) game->camera->setPanningVariables(stadiums[0].get());
}

void ActiveState::cleanup()
{
    glClearColor(imguiColor[0], imguiColor[1], imguiColor[2], 1.00f);
    delete floor;
}

void ActiveState::pause() {}

void ActiveState::resume() {}

void ActiveState::handleEvents() {
    if (game->im.keyJustPressed(GLFW_KEY_TAB)) {
        showInfoScreen = !showInfoScreen;
    }

    if (game->im.keyJustPressed(GLFW_KEY_D) && glfwGetKey(game->getWindow(), GLFW_KEY_LEFT_CONTROL)) {
        game->debugMode = !game->debugMode;
        std::cout << "Debug mode is " << (game->debugMode ? "On" : "Off") << std::endl;
    }

    if (game->im.keyJustPressed(CtrlD)) {
        game->debugMode = !game->debugMode;
        std::cout << "Debug mode is " << (game->debugMode ? "On" : "Off") << std::endl;
    }

    for (const auto& [movementKey, action] : movementKeys) {
        if (game->im.keyPressed(movementKey)) {
            game->camera->processKeyboard(action, game->deltaTime);
        }
    }
    game->camera->update(game->deltaTime);

    if (game->im.mouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        float xpos, ypos; double dx, dy;
        glfwGetCursorPos(game->getWindow(), &dx, &dy);
        xpos = static_cast<float>(dx);
        ypos = static_cast<float>(dy);

        // Calculate world ray and perform intersection check
        vec3 ray_world = screenToWorldCoordinates(game->getWindow(), xpos, ypos,
            game->camera->getViewMatrix(),
            game->projection);
        std::cout << "Left mouse button clicked! Ray: " << ray_world[0] << ", "
            << ray_world[1] << ", " << ray_world[2] << std::endl;
    }

    // RIght click + drag to move camera
    if (game->im.mouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        auto [xOffset, yOffset] = game->im.getMouseOffsets();
        if (xOffset != 0.0f || yOffset != 0.0f) {
            game->camera->processMouseMovement(xOffset, yOffset);
        }
    }

    if (game->im.scrollMoved()) {
        float scrollOffsetY = game->im.getScrollOffsetY();
        game->camera->processMouseScroll(scrollOffsetY);
        game->im.resetScrollOffset();
    }
}

void ActiveState::onResize(int width, int height) {}

void ActiveState::update(float deltaTime) {
    game->physicsWorld->advance(deltaTime);
}


void ActiveState::draw() {
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
    float alpha = game->physicsWorld->getInterpolationAlpha();
    for (const shared_ptr<Beyblade>& beyblade : beyblades) beyblade->render(*objectShader, alpha);


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
    ImGui::Text("%s", positionText.c_str());
    ImGui::End();
}


void ActiveState::drawInfoScreen() {
    SetWindowPositionAndSize(3, 4, 2, 1, 2, 1);

    ImGui::Begin("Settings and Launch Menu");

    // Pause button
    ImGui::SameLine();
    if (ImGui::Button("Pause")) {
        ImGui::End();
        game->pushState(StateFactory::createState(game, GameStateType::PAUSE));
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

    for (size_t i = 0; i < beyblades.size(); ++i) {
        BeybladeBody* beybladeBody = beyblades[i]->getBody();
        ImGui::PushID(static_cast<int>(i));  // Names alone collide across beyblades
        if (ImGui::CollapsingHeader(beyblades[i]->getName().data())) {
            vec3 center = beybladeBody->getCenter().value();
            vec3 velocity = beybladeBody->getVelocity().value();
            vec3 angularVelocity = beybladeBody->getAngularVelocity().value();
            ImGui::Text("Center:   %.3f, %.3f, %.3f", center.x, center.y, center.z);
            ImGui::Text("Velocity: %.3f, %.3f, %.3f", velocity.x, velocity.y, velocity.z);
            ImGui::Text("Spin:     %.1f, %.1f, %.1f", angularVelocity.x, angularVelocity.y, angularVelocity.z);
        }
        ImGui::PopID();
    }

    ImGui::Checkbox("Bound Camera", &game->boundCamera);

    CameraMode currentMode = game->camera->activeMode;
    if (ImGui::RadioButton("Free", currentMode == CameraMode::FREE)) {
        game->camera->changeCameraMode(CameraMode::FREE);
        game->ml.addMessage("Camera changed to free", MessageType::Normal, true);
    }
    if (ImGui::RadioButton("Attached", currentMode == CameraMode::ATTACHED)) {
        game->camera->changeCameraMode(CameraMode::ATTACHED);
        game->ml.addMessage("Camera attached to bey" + std::string(game->pm.getActiveProfile()->getName()), MessageType::Normal, true);
    }
    if (ImGui::RadioButton("Panning", currentMode == CameraMode::PANNING)) {
        game->camera->changeCameraMode(CameraMode::PANNING);
        game->ml.addMessage("Camera changed to pan", MessageType::Normal, true);
    }

    ImGui::End();
}