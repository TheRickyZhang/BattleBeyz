#include "AboutState.h"

#include "GameEngine.h"
#include "FontManager.h"
#include "StateIdentifiers.h"
#include "UI.h"

using namespace ImGui;
using namespace std;

class GameEngine;

void AboutState::init() {}
void AboutState::cleanup() {}
void AboutState::pause() {}
void AboutState::resume() {}
void AboutState::handleEvents() {}
void AboutState::onResize(int width, int height) {}
void AboutState::update(float deltaTime) {}

void AboutState::draw() {
    auto [centerX, wrapWidth] = SetWindowPositionAndSize(1, 1, 1, 1);
    Begin("About Screen", nullptr, MinimalWindow);
    renderBackground(game, "defaultBackground");

    float windowWidth = static_cast<float>(game->windowWidth);

    // Title Section
    PushFont(game->fm.getFont("title"));
    centerWrappedText(windowWidth/2, wrapWidth, "About BattleBeyz");
    PopFont();

    SeparatorSpaced();

    // Section with a link to the GitHub repository
    {
        const char* part1 = "This project is currently in development. If you want to contribute, feel free to fork the repo";
        const char* linkText = "here";
        float totalWidth = CalcTextSize(part1).x + CalcTextSize(linkText).x;
        SetCursorPosX(centerX - totalWidth / 2.0f);
        Text(part1);
        SameLine(); textWithLink(linkText, "https://github.com/TheRickyZhang/BattleBeyz");
    }

    SeparatorSpaced();

    {
        const char* part1 = "Like what you see? Show your support by getting some hand-crafted Lego Beyblades on";
        const char* linkText = "my website";
        float totalWidth = CalcTextSize(part1).x + CalcTextSize(linkText).x;
        SetCursorPosX(centerX - totalWidth / 2.0f);
        Text(part1);
        SameLine(); textWithLink(linkText, "https://www.brickbeyz.com/shop");
    }
    centerWrappedText(centerX, wrapWidth, "For any inquiries, please contact me at rickyzhang196@outlook.com.");

    SeparatorSpaced();

    centerWrappedText(centerX, wrapWidth, "Special thanks to Bill for helping with development. This would not be possible without them!");

    SeparatorSpaced();

    centerWrappedText(centerX, wrapWidth, "Thank you for playing BattleBeyz!");
    centerWrappedText(centerX, wrapWidth, "(c) 2024 Ricky Zhang. All rights reserved.");

    SeparatorSpaced();

    if (centerButton(centerX, "Back to Home")) {
        game->changeState(StateFactory::createState(game, GameStateType::HOME));
    }
    End();
}