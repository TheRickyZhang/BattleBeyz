#include <random>

#include "LoadingState.h"

#include "GameEngine.h"
#include "FontManager.h"

#include "ImGuiUI.h"

using namespace std;
using namespace ImGui;

void LoadingState::init() {
    static thread_local std::mt19937 rng{ std::random_device{}() };
    loadingMessage = tips[std::uniform_int_distribution<size_t>(0, tips.size() - 1)(rng)];

    progress = make_shared<LoadProgress>();
    progress->total.store(tasks.size(), std::memory_order_relaxed);

    // Tasks are independent, so submit them all and let the pool decide the order
    for (const auto& task : tasks) {
        game->jobs.submit([state = progress, task]() {
            if (state->cancelled.load(std::memory_order_relaxed)) return;

            bool succeeded = false;
            try {
                succeeded = task();
            }
            catch (...) {
                succeeded = false;
            }
            if (!succeeded) state->failed.store(true, std::memory_order_relaxed);
            state->completed.fetch_add(1, std::memory_order_relaxed);
        });
    }
    tasks.clear();  // The jobs hold their own copies now
}

void LoadingState::cleanup() {
    // Queued jobs will see this and return immediately. In-flight ones finish against
    // the shared progress block, so there is nothing here to wait on.
    if (progress) progress->cancelled.store(true, std::memory_order_relaxed);
}

void LoadingState::pause() {}
void LoadingState::resume() {}
void LoadingState::handleEvents() {}
void LoadingState::onResize(int width, int height) {}

void LoadingState::update(float deltaTime) {
    if (completionFired || !progress) return;
    if (progress->failed.load(std::memory_order_relaxed)) return;

    size_t total = progress->total.load(std::memory_order_relaxed);
    if (progress->completed.load(std::memory_order_relaxed) < total) return;

    completionFired = true;
    if (onComplete) onComplete();
}

void LoadingState::draw() {
    renderBackground(game, "defaultBackground");

    auto [centerX, wrapWidth] = SetWindowPositionAndSize(3, 4, 2, 2, 1, 2);

    Begin("Loading Screen", nullptr, MinimalWindow);

    PushFont(game->fm.getFont("title"));
    centerWrappedText(centerX, wrapWidth, "Loading...");
    PopFont();

    size_t total = progress ? progress->total.load(std::memory_order_relaxed) : 0;
    size_t done = progress ? progress->completed.load(std::memory_order_relaxed) : 0;
    bool failed = progress && progress->failed.load(std::memory_order_relaxed);
    float fraction = total > 0 ? static_cast<float>(done) / static_cast<float>(total) : 1.0f;

    centerWrappedText(centerX, wrapWidth, failed
        ? "Failed to load assets. Returning to previous screen."
        : loadingMessage.c_str());
    centerProgressBar(centerX, 0.8f * wrapWidth, fraction, "Loading...");

    if (failed && ImGui::Button("Return")) {
        game->popState();
    }

    End();
}
