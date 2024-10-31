// MessageLog.cpp
#include "MessageLog.h"
#include "imgui.h"

MessageLog::MessageLog() : visible(false) {}

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

void MessageLog::addMessage(const std::string& text, MessageType type) {
    messageLog.emplace_back(text, type);
}

void MessageLog::render() {
    if (!visible) return;

    // ImGui window flags to customize the window
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav;

    // Position the window at the bottom-left corner
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 window_pos = ImVec2(0, io.DisplaySize.y);
    ImVec2 window_pos_pivot = ImVec2(0, 1);

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

    ImGui::Begin("Message Log", nullptr, window_flags);

    // Optionally limit the height to make it scrollable
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 150), false, ImGuiWindowFlags_HorizontalScrollbar);

    // Display each message
    for (const auto& message : messageLog) {
        ImGui::TextColored(message.color, "%s", message.text.c_str());
    }

    // Auto-scroll to the bottom
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();

    // Optional: Clear messages
    if (ImGui::Button("Clear")) {
        messageLog.clear();
    }

    ImGui::End();
}
