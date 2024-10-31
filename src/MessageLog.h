// MessageLog.h
#pragma once
#include <vector>
#include <string>
#include "GameMessage.h"
#include "imgui.h"

class MessageLog {
public:
    MessageLog();
    
    void open();
    void close();
    void toggle();
    bool isOpen() const;

    void addMessage(const std::string& text, MessageType type);
    void displayMessages();

    void render();
private:
    bool visible;
    std::vector<GameMessage> messageLog;
};
