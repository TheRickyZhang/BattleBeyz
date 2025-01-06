#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>

#include "MessageLog.h"
#include "UI.h"

using namespace std;

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

void MessageLog::addMessage(const string& text, MessageType type, bool overwrite) {
    messageLog.emplace_back(text, type);
    // Only show the log on warnings or higher
    if (overwrite || type >= MessageType::WARNING) {
        open();
    }
}

// Displays the message log on screen with all the messages
void MessageLog::render() {
    if (!visible) return;   
    SetWindowPositionAndSize(2, 4, 1, 4);
    ImGui::Begin("Message Log", nullptr, NormalWindow);

    // TODO: Make sure these buttons stay while below messaegs scroll
    if (ImGui::Button("Clear")) messageLog.clear();
    ImGui::SameLine();
    if (ImGui::Button("Close")) visible = false;
    ImGui::Separator();

    // Display each message, auto scroll to bottom
    ImGui::BeginChild("ScrollRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
    for (const auto& message : messageLog) {
        ImGui::TextColored(message.color, "%s", message.text.c_str());
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

    // Format the time into a readable string
    ostringstream oss;
    oss << put_time(&timeInfo, "%Y-%m-%d_%H-%M-%S");

    string fileName = path + "/log_" + oss.str() + ".txt";
    ofstream outFile(fileName);
    if (!outFile.is_open()) {
        cerr << "Error: Could not open log file for writing: " << fileName << endl;
        return;
    }

    for (const auto& message : messageLog) {
        outFile << "[" << message.getTypeString() << "] " << message.text << endl;
    }

    outFile.close();
    cout << "Log uploaded successfully to: " << fileName << endl;
}