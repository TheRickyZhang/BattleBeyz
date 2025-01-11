#include <chrono>

#include "LoadingState.h"

#include "GameEngine.h"
#include "FontManager.h"

#include "UI.h"

using namespace std;
using namespace ImGui;

void LoadingState::init() {
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        loadingMessage = tips[(rand() * tips.size()) % tips.size()];
    }
    taskThread = std::thread([this]() {
        for (taskIndex = 0; taskIndex < tasks.size(); ++taskIndex) {
            if (!tasks[taskIndex]()) {
                {
                    std::lock_guard<std::mutex> lock(dataMutex);
                    failed = true;
                    loadingMessage = "Failed to load assets. Returning to previous screen.";
                }
                break;
            }
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                progress = float(taskIndex + 1) / tasks.size();
            }
            //this_thread::sleep_for(chrono::milliseconds(100));
        }
        std::lock_guard<std::mutex> lock(dataMutex);
        completed = true;  // Moved after onComplete() to ensure tasks finished.
    });
}


void LoadingState::cleanup() {
    // Avoid joining if calling thread is the same as taskThread.
    if (taskThread.joinable() && taskThread.get_id() != std::this_thread::get_id()) {
        taskThread.join();
    }
    std::lock_guard<std::mutex> lock(dataMutex);
    loadingMessage = "";
}

void LoadingState::pause() {}
void LoadingState::resume() {}
void LoadingState::handleEvents() {}

void LoadingState::update(float deltaTime) {
    if (completed) {
        onComplete();
    }
}

void LoadingState::draw() {
    renderBackground(game, "defaultBackground");

    auto [centerX, wrapWidth] = SetWindowPositionAndSize(3, 4, 2, 2, 1, 2);

    Begin("Loading Screen", nullptr, MinimalWindow);

    PushFont(game->fm.getFont("title"));
    centerWrappedText(centerX, wrapWidth, "Loading...");
    PopFont();

    {
        std::lock_guard<std::mutex> lock(dataMutex);
        centerWrappedText(centerX, wrapWidth, loadingMessage.c_str());
        centerProgressBar(centerX, 0.8f * wrapWidth, progress, "Loading...");
    }

    if (failed && ImGui::Button("Return")) {
        game->popState();
    }

    End();
}
