#include "PreBattleState.h"

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

#include "InteractiveBehavior.h"
#include "Utils.h"

using namespace std;
using namespace glm;


void PreBattleState::init()
{
    PhysicsWorld* physicsWorld = game->physicsWorld.get();

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
    // Spread the beys evenly around a ring rather than stacking them at one point.
    // Spin defaults well above MIN_SPIN_THRESHOLD so a launch actually simulates.
    const float ringRadius = 0.3f;
    const float startHeight = 1.0f;
    const float inwardSpeed = 0.1f;
    const float startSpin = -450.0f;

    launchSettings.clear();
    for (size_t i = 0; i < beyblades.size(); ++i) {
        float angle = (2.0f * glm::pi<float>() * i) / beyblades.size();
        vec3 position(ringRadius * cosf(angle), startHeight, ringRadius * sinf(angle));
        vec3 inward = -normalize(vec3(position.x, 0.0f, position.z));

        launchSettings.push_back({ position, inwardSpeed * inward, vec3(0.0f, startSpin, 0.0f) });

        beyblades[i]->getBody()->resetPhysics(Vec3_M(position));
        physicsWorld->addBeyblade(beyblades[i].get());
    }
}

void PreBattleState::cleanup()
{
    delete floor;
    floor = nullptr;
}

void PreBattleState::pause() {}

void PreBattleState::resume() {}

void PreBattleState::handleEvents() {
    GLFWwindow* window = game->getWindow();
    Camera* camera = game->camera;

    // --- Picking Phase: On initial left click, try to select a beyblade ---
    if (game->im.mouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT) && heldBeyblade < 0) {
        double dx, dy;
        glfwGetCursorPos(window, &dx, &dy);
        float xpos = static_cast<float>(dx);
        float ypos = static_cast<float>(dy);

        glm::vec3 rayDir = screenToWorldCoordinates(window, xpos, ypos,
            camera->getViewMatrix(),
            game->projection);
        rayDir = glm::normalize(rayDir);
        glm::vec3 rayOrigin = camera->position;

        int selectedBeyblade = -1;
        glm::vec3 hitIntersection;
        float closestT = FLT_MAX;

        for (size_t i = 0; i < beyblades.size(); ++i) {
            float t;
            if (rayIntersectsAABB(rayOrigin, rayDir, beyblades[i]->getBody()->getBoundingBox(), t)) {
                if (t < closestT) {
                    closestT = t;
                    selectedBeyblade = static_cast<int>(i);
                    hitIntersection = rayOrigin + t * rayDir;
                }
            }
        }

        if (selectedBeyblade >= 0) {
            heldBeyblade = selectedBeyblade;
            // Save offset so the object "sticks" relative to the pointer
            dragOffset = launchSettings[heldBeyblade].center - hitIntersection;
        }
    }

    // --- Dragging Phase: While left button is held, move the beyblade ---
    if (game->im.mouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && heldBeyblade >= 0) {
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
            launchSettings[heldBeyblade].center = currentIntersection + dragOffset;
        }
    }

    // --- Release Phase: When left mouse button is released, stop dragging ---
    if (game->im.mouseButtonJustReleased(GLFW_MOUSE_BUTTON_LEFT)) {
        heldBeyblade = -1;
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
    // No physics step here: this screen only positions the beys. Keep the bodies
    // in sync with the settings so dragging and the sliders both show up on screen.
    for (size_t i = 0; i < beyblades.size(); ++i) {
        beyblades[i]->getBody()->setCenter(launchSettings[i].center);
    }
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
    for (const shared_ptr<Beyblade>& beyblade : beyblades) beyblade->render(*objectShader);


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


void PreBattleState::drawInfoScreen() {
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

    // Ranges are sized to the stadium and to MIN/MAX_SPIN_THRESHOLD, not arbitrary.
    const float centerRange = 1.0f;
    const float velocityRange = 10.0f;
    const float spinRange = 1500.0f;

    for (size_t i = 0; i < beyblades.size(); ++i) {
        ImGui::PushID(static_cast<int>(i));  // Names alone collide across beyblades
        if (ImGui::CollapsingHeader(beyblades[i]->getName().data())) {
            LaunchSettings& settings = launchSettings[i];

            ImGui::Text("Center");
            ImGui::SliderFloat("X##CTR", &settings.center.x, -centerRange, centerRange);
            ImGui::SliderFloat("Y##CTR", &settings.center.y, -centerRange, centerRange);
            ImGui::SliderFloat("Z##CTR", &settings.center.z, -centerRange, centerRange);

            ImGui::Text("Velocity");
            ImGui::SliderFloat("X##V", &settings.velocity.x, -velocityRange, velocityRange);
            ImGui::SliderFloat("Y##V", &settings.velocity.y, -velocityRange, velocityRange);
            ImGui::SliderFloat("Z##V", &settings.velocity.z, -velocityRange, velocityRange);

            ImGui::Text("Angular Velocity");
            ImGui::SliderFloat("X##AV", &settings.angularVelocity.x, -spinRange, spinRange);
            ImGui::SliderFloat("Y##AV", &settings.angularVelocity.y, -spinRange, spinRange);
            ImGui::SliderFloat("Z##AV", &settings.angularVelocity.z, -spinRange, spinRange);
        }
        ImGui::PopID();
    }

    if (ImGui::Button("Launch")) {
        for (size_t i = 0; i < beyblades.size(); ++i) {
            const LaunchSettings& settings = launchSettings[i];
            beyblades[i]->getBody()->setInitialLaunch(
                Vec3_M(settings.center), settings.velocity, settings.angularVelocity);
        }
        ImGui::End();
        game->changeState(std::make_unique<ActiveState>(game, stadiums, beyblades));
        return;
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