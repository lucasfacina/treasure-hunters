#include "managers/game_manager.h"
#include "game_screen.h"

#include "game_over_screen.h"

void GameScreen::_triggerEndMatch() {
    auto matchEndAt = std::chrono::steady_clock::now();
    auto durationInSeconds = std
            ::chrono
            ::duration_cast<std::chrono::seconds>(matchEndAt - matchStartAt)
            .count();

    this->gameManager->pushScreen(std::make_shared<GameOverScreen>(GameOverInfo{
        .blueScore = std::stoi(map_manager->getScore(BLUE)),
        .pinkScore = std::stoi(map_manager->getScore(PINK)),
        .matchDurationInSeconds = durationInSeconds,
        .houseSlotByPlayerType = map_manager->getHouseSlotByPlayerType()
    }));
}

