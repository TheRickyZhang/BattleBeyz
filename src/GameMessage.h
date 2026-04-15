#pragma once

#include <array>
#include <string>
#include <stdexcept>

#include <imgui.h>

// Not SCREAMING_CASE: windows.h defines ERROR as a macro, which would rewrite
// the enumerator out from under this header wherever GL/glew.h is included first.
enum class MessageType {
    Normal,
    Error,
    Warning,
    Success
};

static const std::array<std::string, 4> typeStrings = {
    "NORMAL",
    "ERROR",
    "WARNING",
    "SUCCESS"
};

struct GameMessage {
    std::string text;
    MessageType type;
    ImVec4 color;

    GameMessage(const std::string& message, MessageType messageType)
        : text(message), type(messageType), color(getDefaultColor(messageType)) {}

    std::string getTypeString() const {
        size_t index = static_cast<size_t>(type);
        if (index >= typeStrings.size()) throw std::out_of_range("Invalid MessageType index");
        return typeStrings[index];
    }

private:
    static ImVec4 getDefaultColor(MessageType messageType) {
        switch (messageType) {
            case MessageType::Error:   return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red for errors
            case MessageType::Warning: return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow for warnings
            case MessageType::Success: return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green for success
            default:                   return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White for normal
        }
    }
};