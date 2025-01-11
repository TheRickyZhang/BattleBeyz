#pragma once

#include <thread>
#include <mutex>

#include "GameState.h"

class LoadingState : public GameState {
public:
    LoadingState(GameEngine* _game, const std::vector<std::function<bool()>>& tasks, std::function<void()> onComplete) :
        GameState(_game), tasks(tasks), onComplete(onComplete) {};

    void init() override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvents() override;
    void update(float deltaTime) override;
    void draw() override;

    GameStateType getStateType() const override { return GameStateType::LOADING; }

private:
    // TODO: add vector<string> assetsToLoad to determine what actually needs to be loaded
    float timeElapsed = 0.0f;

    float progress = 0.0f;
    const std::vector<std::string> tips = {
        "Tip: Customize your Beyblade for maximum power!",
        "Did you know: you can upload your own beyblade as an .obj file! See the customization screen for more details",
        "Choose from over 100 unique template combinations, or upload your own unique bey!",
        "Have feedback or want to contribute? Get in contact at \"Battlebeyz\"."
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
    std::string loadingMessage;

    bool failed = false;
    bool completed = false;
    int taskIndex = 0;

    std::mutex dataMutex;   // New mutex for protecting shared data
    std::thread taskThread;
    std::vector<std::function<bool()>> tasks;
    std::function<void()> onComplete;
};
