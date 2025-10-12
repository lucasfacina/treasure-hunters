#ifndef GAME_OVER_INFO_H
#define GAME_OVER_INFO_H

#include <map>
#include <vector>
#include <memory>
#include "objects/house_slot.h"
#include "utils/player_type.h"

struct GameOverInfo {
    int blueScore;
    int pinkScore;
    long long matchDurationInSeconds;
    std::map<PlayerType, std::vector<std::shared_ptr<HouseSlotObject>>> houseSlotByPlayerType;
};

#endif //GAME_OVER_INFO_H
