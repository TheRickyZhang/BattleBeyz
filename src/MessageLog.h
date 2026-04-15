#pragma once

#include <atomic>
#include <deque>
#include <mutex>
#include <string>

#include "GameMessage.h"

/*
* Singleton
*
* Provides a log for easier user-facing and debugging messages.
*
* addMessage() is safe to call from any thread; render() and save() are main-thread only.
*/
class MessageLog {
public:
    static constexpr size_t MAX_MESSAGES = 1000;

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

    void addMessage(const std::string& text, MessageType type = MessageType::Normal, bool showLog = false);
    void render();
    void save(const std::string& path);
private:
    MessageLog() : visible(false) {}
    ~MessageLog() = default;

    std::atomic<bool> visible;
    mutable std::mutex mutex;
    std::deque<GameMessage> messageLog;  // Oldest dropped past MAX_MESSAGES
};
