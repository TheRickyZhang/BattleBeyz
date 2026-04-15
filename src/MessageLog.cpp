#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <filesystem>

#include "MessageLog.h"
#include "ImGuiUI.h"

using namespace std;

void MessageLog::open() {
    visible = true;
}

void MessageLog::close() {
    visible = false;
}

void MessageLog::toggle() {
    visible = !visible.load();
}

bool MessageLog::isOpen() const {
    return visible;
}

void MessageLog::addMessage(const string& text, MessageType type, bool overwrite) {
    {
        lock_guard<std::mutex> lock(mutex);
        messageLog.emplace_back(text, type);
        while (messageLog.size() > MAX_MESSAGES) {
            messageLog.pop_front();
        }
    }
    // Only show the log on warnings or higher
    if (overwrite || type >= MessageType::Warning) {
        open();
    }
}

// Displays the message log on screen with all the messages
void MessageLog::render() {
    if (!visible) return;   
    SetWindowPositionAndSize(2, 4, 1, 4);
    ImGui::Begin("Message Log", nullptr, ImGuiWindowFlags_None);

    // TODO: Make sure these buttons stay while below messaegs scroll
    bool clearRequested = ImGui::Button("Clear");
    ImGui::SameLine();
    if (ImGui::Button("Close")) visible = false;
    ImGui::Separator();

    // Display each message, auto scroll to bottom
    ImGui::BeginChild("ScrollRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
    {
        lock_guard<std::mutex> lock(mutex);
        if (clearRequested) messageLog.clear();
        for (const auto& message : messageLog) {
            ImGui::TextColored(message.color, "%s", message.text.c_str());
        }
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();

    ImGui::End();
}


void MessageLog::save(const string& path) {
    auto now = time(nullptr);
    tm timeInfo;
#ifdef _WIN32
    localtime_s(&timeInfo, &now);
#else
    localtime_r(&now, &timeInfo);
#endif

    // Snapshot so the file write does not hold the lock
    deque<GameMessage> messages;
    {
        lock_guard<std::mutex> lock(mutex);
        messages = messageLog;
    }

    for (const auto& message : messages) {
        std::cout << "[" << message.getTypeString() << "] " << message.text << std::endl;
    }
    // Ensure the directory exists
    try {
        if (!filesystem::exists(path)) {
            filesystem::create_directories(path);
        }
    }
    catch (const filesystem::filesystem_error& e) {
        cerr << "Error: Could not create directory: " << e.what() << endl;
        return;
    }

    // Format the time into a readable string
    ostringstream oss;
    oss << put_time(&timeInfo, "%Y-%m-%d_%H-%M-%S");

    string fileName = path + "/log_" + oss.str() + ".txt";
    ofstream outFile(fileName);
    if (!outFile.is_open()) {
        cerr << "Error: Could not open log file for writing: " << fileName << endl;
        return;
    }

    for (const auto& message : messages) {
        outFile << "[" << message.getTypeString() << "] " << message.text << endl;
    }

    outFile.flush();
    outFile.close();
    cout << "Log uploaded successfully to: " << fileName << endl;
}