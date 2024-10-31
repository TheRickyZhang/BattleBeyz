#pragma once

#include <string>
#include <imgui.h>

enum class MessageType {
    NORMAL,
    ERROR,
    WARNING,
    SUCCESS
};


struct GameMessage {
    std::string text;
    MessageType type;
    ImVec4 color;

    GameMessage(const std::string& message, MessageType messageType)
        : text(message), type(messageType), color(getDefaultColor(messageType)) {}

private:
    static ImVec4 getDefaultColor(MessageType messageType) {
        switch (messageType) {
            case MessageType::ERROR:   return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red for errors
            case MessageType::WARNING: return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow for warnings
            case MessageType::SUCCESS: return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green for success
            default:                   return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White for normal
        }
    }
};