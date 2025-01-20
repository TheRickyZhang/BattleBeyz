#include "RenderUI.h"
#include <imgui.h>
#include <glm/ext/matrix_clip_space.hpp>

StadiumPreview::StadiumPreview(int width, int height, PhysicsWorld* physicsWorld)
    : previewWidth(width), previewHeight(height) {
    camera = std::make_unique<Camera>(
        glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f), physicsWorld, static_cast<float>(width), static_cast<float>(height));
    stadiumRenderer = std::make_unique<FramebufferRenderer>(width, height);
}

void StadiumPreview::init() {
    auto tempStadium = std::make_unique<Stadium>();
    stadium = std::make_unique<Stadium>(*tempStadium);

    stadiumRenderTimer = std::make_unique<Timer>(0.15f, [this]() {
        stadium->updateMesh();
        });
}

void StadiumPreview::update(float deltaTime, float currentTime) const {
    camera->update(deltaTime);
    if (stadiumRenderTimer->shouldTrigger(currentTime)) {
        stadiumRenderTimer->trigger(currentTime);
    }
}

bool StadiumPreview::draw(ObjectShader& shader) {
    stadiumRenderer->bind();
    glViewport(0, 0, previewWidth, previewHeight);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.setMat4("projection", glm::perspective(glm::radians(camera->zoom), (float)previewWidth / previewHeight, 0.1f, 100.0f));
    shader.setMat4("view", camera->getViewMatrix());
    stadium->render(shader);

    stadiumRenderer->unbind();


    ImGui::BeginChild("StadiumPreview", ImVec2((float)previewWidth, (float)previewHeight), false, ImGuiWindowFlags_NoScrollbar);
    isMouseHovering = ImGui::IsItemHovered();
    ImGui::Image((void*)(intptr_t)stadiumRenderer->getTexture(), ImVec2((float)previewWidth, (float)previewHeight));
    ImGui::EndChild();

    return isMouseHovering;
}
