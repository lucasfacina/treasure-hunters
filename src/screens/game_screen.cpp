#include "managers/game_manager.h"
#include "game_screen.h"

#include "game_over_screen.h"

void GameScreen::_triggerEndMatch() {
    // Congela o cronômetro da tela da partida enquanto o Game Over está sobreposto
    this->matchEndAt = std::chrono::steady_clock::now();
    const auto durationInSeconds = this->_getMatchElapsedSeconds();

    this->gameManager->pushScreen(std::make_shared<GameOverScreen>(GameOverInfo{
        .blueScore = std::stoi(map_manager->getScore(BLUE)),
        .pinkScore = std::stoi(map_manager->getScore(PINK)),
        .matchDurationInSeconds = durationInSeconds,
        .isRanked = isRankedMatch,
        .houseSlotByPlayerType = map_manager->getHouseSlotByPlayerType()
    }));
}

