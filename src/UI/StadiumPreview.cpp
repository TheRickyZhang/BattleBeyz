﻿#include "StadiumPreview.h"
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include "ObjectShader.h"
#include "InputUtils.h"

using namespace std;
StadiumPreview::StadiumPreview(int width, int height, PhysicsWorld* physicsWorld, ObjectShader* shader)
    : width(width), height(height), physicsWorld(physicsWorld), objectShader(shader)
{
    fbo = std::make_unique<FramebufferRenderer>(width, height);
    camera = std::make_unique<Camera>(glm::vec3(5.f, 5.f, 5.f),
        glm::vec3(0.f, 0.f, 0.f),
        this->physicsWorld,
        static_cast<float>(width),
        static_cast<float>(height));
    stadium = new Stadium();

    // Re‐generates the stadium mesh every 0.15 seconds so performance is not severly hindered
    stadiumRenderTimer = std::make_unique<Timer>(0.15f, [this]() {
        if (stadium) stadium->updateMesh();
    });
}

void StadiumPreview::handleInput(float deltaTime) {
    // Only rotate camera if ImGui says we are hovering over the preview
    if (!hovered) return;

    // Mouse drag to rotate
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        camera->processMouseMovement(delta.x, -delta.y);
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
    }

    // Mouse wheel to zoom
    if (ImGui::GetIO().MouseWheel != 0.0f) {
        camera->processMouseScroll(ImGui::GetIO().MouseWheel);
    }

    // Optional WASD input
    if (ImGui::IsKeyPressed(ImGuiKey_W)) camera->processKeyboard(Action::MoveForward, deltaTime);
    if (ImGui::IsKeyPressed(ImGuiKey_S)) camera->processKeyboard(Action::MoveBackward, deltaTime);
    if (ImGui::IsKeyPressed(ImGuiKey_A)) camera->processKeyboard(Action::MoveLeft, deltaTime);
    if (ImGui::IsKeyPressed(ImGuiKey_D)) camera->processKeyboard(Action::MoveRight, deltaTime);
}

void StadiumPreview::update(float deltaTime, float currentTime) {
    //if (!isLoading && tempStadium) {
    //    std::lock_guard<std::mutex> lock(stadiumMutex);
    //    stadium = tempStadium; // Safely swap the loaded stadium
    //}
    camera->update(deltaTime);
    if (stadiumRenderTimer->shouldTrigger(currentTime)) {
        stadiumRenderTimer->trigger(currentTime);
    }
}

void StadiumPreview::draw() {
    //if (isLoading) {
    //    ImGui::BeginChild("StadiumPreview", ImVec2((float)width, (float)height), false, ImGuiWindowFlags_NoScrollbar);
    //    ImGui::Text("Loading...");
    //    ImGui::SameLine();
    //    ImGui::Text("Please wait.");
    //    ImGui::EndChild();
    //    return;
    //}

    fbo->bind();
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (objectShader) {
        objectShader->use();
        objectShader->setMat4(
            "projection",
            glm::perspective(glm::radians(camera->zoom),
                (float)width / (float)height,
                0.1f,
                100.0f)
        );
        objectShader->setMat4("view", camera->getViewMatrix());
        if (stadium) {
            stadium->render(*objectShader);
        }
    }

    fbo->unbind();
    // TODO:
    // Restore main viewport afterwards. Do in cleanup?

    ImGui::BeginChild("StadiumPreview", ImVec2((float)width, (float)height), false, ImGuiWindowFlags_NoScrollbar);
    hovered = ImGui::IsItemHovered(); // We'll use this in handleInput
    ImGui::Image((void*)(intptr_t)fbo->getTexture(), ImVec2((float)width, (float)height));
    ImGui::EndChild();
}

// TODO: Run stadium rendering on separate thread and show loading screen
//void StadiumPreview::updateStadiumAsync(Stadium* newStadium) {
//    if (isLoading) return;
//
//    isLoading = true; // Mark as loading
//    loadingThread = std::thread([this, newStadium = newStadium]() mutable {
//        {
//            std::lock_guard<std::mutex> lock(stadiumMutex);
//            tempStadium = newStadium;
//        }
//        isLoading = false;
//    });
//    loadingThread.detach(); 
//}