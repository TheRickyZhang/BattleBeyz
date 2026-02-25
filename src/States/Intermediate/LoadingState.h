#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "GameState.h"

/*
* Runs a batch of CPU-side tasks on the job system while keeping the window responsive.
*
* Tasks must not call OpenGL: they run on worker threads. Produce plain data and push
* the upload through GameEngine::glUploads, which runs on the main thread.
*/
class LoadingState : public GameState {
public:
    LoadingState(GameEngine* _game, const std::vector<std::function<bool()>>& tasks, std::function<void()> onComplete) :
        GameState(_game), tasks(tasks), onComplete(std::move(onComplete)) {};

    void init() override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvents() override;
    void onResize(int width, int height) override;

    void update(float deltaTime) override;
    void draw() override;

    GameStateType getStateType() const override { return GameStateType::LOADING; }

private:
    // Shared with the jobs so it outlives this state; the jobs never touch `this`,
    // which is what lets cleanup() return without waiting on in-flight work.
    struct LoadProgress {
        std::atomic<size_t> completed{ 0 };
        std::atomic<size_t> total{ 0 };
        std::atomic<bool> failed{ false };
        std::atomic<bool> cancelled{ false };
    };
    std::shared_ptr<LoadProgress> progress;

    const std::vector<std::string> tips = {
        "Tip: Customize your Beyblade for maximum power!",
        "Did you know: you can upload your own beyblade as an .obj file! See the customization screen for more details",
        "Choose from over 100 unique template combinations, or upload your own unique bey!",
        "Have feedback or want to contribute? Get in contact at \"Battlebeyz\".",
        "Plastic, Metal, Burst, X, Lego. Hmmmm...",
        "Want to support this game? Visit brickbeyz.com to get your own lego beyblades!",
        "Did you know: the phrase \"spin to win\" actually originates from Beyblade. Jk.",
        "The curtains rise on me, this is my destiny...",
        "Rigid body mechanics: Hold my rotational inertia",
        "cringe.",
        "LGBTQ? More like Let's Go Beyblades Traveling Quick hehe",
        "I wonder what the official Beyblade games are like...",
        "loading..................................."
    };
    std::string loadingMessage;  // Main thread only

    bool completionFired = false;  // Main thread only; onComplete must run once

    std::vector<std::function<bool()>> tasks;
    std::function<void()> onComplete;
};
