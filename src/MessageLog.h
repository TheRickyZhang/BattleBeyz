#pragma once

#include <vector>
#include <string>

#include "GameMessage.h"

/*
* Singleton
* 
* Provides a log for easier user-facing and debugging messages.
*/
class MessageLog {
public:
    static MessageLog& getInstance() {
        static MessageLog instance;
        return instance;
    }
    MessageLog(const MessageLog&) = delete;
    MessageLog& operator=(const MessageLog&) = delete;
    
    void open();
    void close();
    void toggle();
    bool isOpen() const;

    void addMessage(const std::string& text, MessageType type = MessageType::NORMAL, bool showLog = false);
    void render();
private:
    MessageLog() : visible(false) {}
    ~MessageLog() = default;

    bool visible;
    std::vector<GameMessage> messageLog;
};
