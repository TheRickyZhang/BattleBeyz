#include "MessageLog.h"
#include "json.hpp"

struct Settings {
    float volume = 0.5f;
    bool isMuted = false;
    int resolutionIndex = 1; // Default: 1280x720
    bool isFullscreen = false;

    nlohmann::json toJson() const {
        return {
            { "volume", volume },
            { "isMuted", isMuted },
            { "resolutionIndex", resolutionIndex },
            { "isFullscreen", isFullscreen }
        };
    }

    void fromJson(const nlohmann::json& json) {
        MessageLog& ml = MessageLog::getInstance();

        // Validate and log issues; set defaults if necessary
        volume = (json.contains("volume") && json["volume"].is_number())
            ? json["volume"]
            : (ml.addMessage("'volume' is missing or not a valid number. Using default value: 1.0", MessageType::WARNING), 1.0f);

        isMuted = (json.contains("isMuted") && json["isMuted"].is_boolean())
            ? json["isMuted"]
            : (ml.addMessage("'isMuted' is missing or not a valid boolean. Using default value: false", MessageType::WARNING), false);

        resolutionIndex = (json.contains("resolutionIndex") && json["resolutionIndex"].is_number_integer())
            ? json["resolutionIndex"]
            : (ml.addMessage("'resolutionIndex' is missing or not a valid integer. Using default value: 0", MessageType::WARNING), 0);

        isFullscreen = (json.contains("isFullscreen") && json["isFullscreen"].is_boolean())
            ? json["isFullscreen"]
            : (ml.addMessage("'isFullscreen' is missing or not a valid boolean. Using default value: false", MessageType::WARNING), false);
    }
};