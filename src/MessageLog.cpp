#include <iostream>
#include "MessageLog.h"
#include "imgui.h"
#include "UI.h"

void MessageLog::open() {
    visible = true;
}

void MessageLog::close() {
    visible = false;
}

void MessageLog::toggle() {
    visible = !visible;
}

bool MessageLog::isOpen() const {
    return visible;
}

void MessageLog::addMessage(const std::string& text, MessageType type, bool showLog) {
    messageLog.emplace_back(text, type);
    if (showLog) open();
}

// Displays the message log on screen with all the messages
void MessageLog::render() {
    if (!visible) return;   
    SetWindowPositionAndSize(2, 4, 1, 4);
    ImGui::Begin("Message Log", nullptr, ScrollingWindow);

    // TODO: Make sure these buttons stay while below messaegs scroll
    if (ImGui::Button("Clear")) messageLog.clear();
    ImGui::SameLine();
    if (ImGui::Button("Close")) visible = false;
    ImGui::Separator();

    // Display each message, auto scroll to bottom
    for (const auto& message : messageLog) {
        ImGui::TextColored(message.color, "%s", message.text.c_str());
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::End();
}
